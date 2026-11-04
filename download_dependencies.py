#!/usr/bin/env python3
"""
Downloads the GameAnalytics SDKs pinned in dependencies.json from their GitHub
releases and installs them into the gameanalytics extension.

  GA-SDK-CPP         desktop "static-no-deps" libs and headers (osx, x86_64-linux, x86_64-win32)
  GA-SDK-IOS         GameAnalytics.framework and its privacy manifest (from the xcframework release)
  GA-SDK-ANDROID     version of the maven dependency in manifests/android/build.gradle
  GA-SDK-JAVASCRIPT  res/web/GameAnalytics.js

Usage:
  python3 download_dependencies.py                 install everything in dependencies.json
  python3 download_dependencies.py --only GA-SDK-CPP --only GA-SDK-IOS
  python3 download_dependencies.py --check         compare pinned versions with the latest releases
  python3 download_dependencies.py --bump          pin the latest releases in dependencies.json, then install

Only the standard library is used. Set GITHUB_TOKEN to avoid the GitHub API
rate limit when using --check or --bump.
"""

import argparse
import fnmatch
import json
import os
import plistlib
import re
import shutil
import sys
import tempfile
import urllib.error
import urllib.request
import zipfile

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
DEPENDENCIES_FILE = os.path.join(ROOT_DIR, "dependencies.json")
EXTENSION_DIR = os.path.join(ROOT_DIR, "gameanalytics")
LIB_DIR = os.path.join(EXTENSION_DIR, "lib")
INCLUDE_DIR = os.path.join(EXTENSION_DIR, "include", "GameAnalytics")
JS_FILE = os.path.join(EXTENSION_DIR, "res", "web", "GameAnalytics.js")
ANDROID_GRADLE_FILE = os.path.join(EXTENSION_DIR, "manifests", "android", "build.gradle")
# Defold links lib/ios for every iOS target, so the device and simulator
# slices each get their own platform folder instead.
IOS_FRAMEWORK_DIRS = {
    "ios-arm64": os.path.join(LIB_DIR, "arm64-ios", "GameAnalytics.framework"),
    "ios-arm64_x86_64-simulator": os.path.join(LIB_DIR, "arm64_sim-ios", "GameAnalytics.framework"),
}
IOS_PRIVACY_MANIFEST = os.path.join(EXTENSION_DIR, "manifests", "ios", "PrivacyInfo.xcprivacy")

# The desktop libs are the "static-no-deps" ones: no curl or OpenSSL, the
# extension sends requests through the engine's HTTP service (GADefold.cpp).
# Folders in the C++ archive are named after the CI runner image (e.g.
# windows-latest-cl-Release became windows-2022-cl-Release), so match by pattern.
# gcc rather than clang on Linux, so the lib links against libstdc++.
CPP_PLATFORMS = [
    ("macOS-*-clang-Release", "libGameAnalytics.a", "osx"),
    ("ubuntu-*-gcc-Release", "libGameAnalytics.a", "x86_64-linux"),
]
# Defold links the static MSVC runtime (/MT), the libs in the regular packages
# use the DLL one (/MD) and fail to link with a RuntimeLibrary mismatch, so
# Windows comes from the "static-mt" package. Releases without it leave the
# vendored lib alone (see README).
CPP_WINDOWS_MT = ("windows-*-cl-Release-static-no_deps", "GameAnalytics.lib", "x86_64-win32")

HEADER_BANNER = (
    "// Vendored copy of include/GameAnalytics/{name} from gameanalytics-sdk-cpp {tag}.\n"
    "// Updated by download_dependencies.py, do not change it here.\n"
    "\n"
)


class DependencyError(Exception):
    pass


def log(message):
    print(message, flush=True)


def repo_slug(dep):
    # "https://github.com/GameAnalytics/GA-SDK-IOS/" -> "GameAnalytics/GA-SDK-IOS"
    match = re.match(r"https://github\.com/([^/]+/[^/]+?)/?$", dep["url"])
    if not match:
        raise DependencyError(f"{dep['name']}: url is not a github repo: {dep['url']}")
    return match.group(1)


def tag_candidates(version):
    # Repos are inconsistent: gameanalytics-sdk-cpp tags "v5.4.1", GA-SDK-IOS
    # tags "5.1.0", and GA-SDK-JAVASCRIPT switched from "4.4.6" to "v5.0.0".
    return [f"v{version}", version]


def request(url, accept=None):
    req = urllib.request.Request(url, headers={"User-Agent": "ga-sdk-defold-dependencies"})
    if accept:
        req.add_header("Accept", accept)
    token = os.environ.get("GITHUB_TOKEN")
    if token and url.startswith("https://api.github.com/"):
        req.add_header("Authorization", f"Bearer {token}")
    return urllib.request.urlopen(req, timeout=120)


def download_first(urls, dest_path):
    """Downloads the first url that exists. Returns the url used."""
    for url in urls:
        try:
            with request(url) as response, open(dest_path, "wb") as f:
                shutil.copyfileobj(response, f)
            log(f"  downloaded {url}")
            return url
        except urllib.error.HTTPError as e:
            if e.code != 404:
                raise DependencyError(f"HTTP {e.code} downloading {url}") from e
        except urllib.error.URLError as e:
            raise DependencyError(f"cannot download {url}: {e.reason}") from e
    raise DependencyError("not found: " + ", ".join(urls))


def download_release_asset(dep, asset_name_for_tag, dest_path):
    """Downloads a release asset, trying both tag spellings. Returns the tag used."""
    base = dep["url"].rstrip("/")
    urls = {f"{base}/releases/download/{tag}/{asset_name_for_tag(tag)}": tag
            for tag in tag_candidates(dep["version"])}
    return urls[download_first(list(urls), dest_path)]


def latest_release(dep):
    url = f"https://api.github.com/repos/{repo_slug(dep)}/releases/latest"
    try:
        with request(url, accept="application/vnd.github+json") as response:
            tag = json.load(response)["tag_name"]
    except urllib.error.HTTPError as e:
        raise DependencyError(f"HTTP {e.code} reading {url}") from e
    return tag[1:] if tag.startswith("v") else tag


def find_one(parent, pattern):
    matches = sorted(n for n in os.listdir(parent) if fnmatch.fnmatch(n, pattern))
    if not matches:
        raise DependencyError(f"no folder matching '{pattern}' in {parent}")
    return os.path.join(parent, matches[0])


def install_cpp(dep, work_dir):
    zip_path = os.path.join(work_dir, "cpp.zip")
    tag = download_release_asset(dep, lambda t: f"ga-sdk-static-no-deps-{t}.zip", zip_path)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(work_dir)
    release_dir = os.path.join(work_dir, "static-no-deps-release")

    def copy_lib(parent_dir, folder_pattern, lib_name, platform):
        src = os.path.join(find_one(parent_dir, folder_pattern), lib_name)
        dst = os.path.join(LIB_DIR, platform, lib_name)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(src, dst)
        log(f"  {os.path.relpath(dst, ROOT_DIR)}")

    for platform in CPP_PLATFORMS:
        copy_lib(release_dir, *platform)

    mt_zip_path = os.path.join(work_dir, "cpp-mt.zip")
    try:
        download_release_asset(dep, lambda t: f"ga-sdk-static-mt-{t}.zip", mt_zip_path)
    except DependencyError:
        log("  WARNING: this release has no ga-sdk-static-mt package, lib/x86_64-win32 is left as it is (see README)")
    else:
        with zipfile.ZipFile(mt_zip_path) as z:
            z.extractall(work_dir)
        copy_lib(os.path.join(work_dir, "static-mt-release"), *CPP_WINDOWS_MT)

    include_src = os.path.join(release_dir, "include", "GameAnalytics")
    os.makedirs(INCLUDE_DIR, exist_ok=True)
    for name in sorted(os.listdir(include_src)):
        with open(os.path.join(include_src, name), "r", encoding="utf-8") as f:
            header = f.read()
        with open(os.path.join(INCLUDE_DIR, name), "w", encoding="utf-8") as f:
            f.write(HEADER_BANNER.format(name=name, tag=tag) + header)
        log(f"  {os.path.relpath(os.path.join(INCLUDE_DIR, name), ROOT_DIR)}")


def install_ios(dep, work_dir):
    zip_path = os.path.join(work_dir, "ios.zip")
    download_release_asset(dep, lambda t: "GameAnalytics.xcframework.zip", zip_path)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(work_dir)
    xcframework = os.path.join(work_dir, "GameAnalytics.xcframework")

    # The extension ships each slice of the xcframework as a plain framework,
    # the device one for arm64-ios and the simulator one for arm64_sim-ios.
    # The privacy manifest is left out of them: the build server only merges
    # privacy manifests from manifests/ios, and only XML ones.
    for slice_name, dst in IOS_FRAMEWORK_DIRS.items():
        src = os.path.join(xcframework, slice_name, "GameAnalytics.framework")
        if not os.path.isdir(src):
            raise DependencyError(f"no {slice_name} framework in {zip_path}")
        if os.path.exists(dst):
            shutil.rmtree(dst)
        shutil.copytree(src, dst, ignore=shutil.ignore_patterns("_CodeSignature", "PrivacyInfo.xcprivacy"))
        log(f"  {os.path.relpath(dst, ROOT_DIR)}")

    device = os.path.join(xcframework, "ios-arm64", "GameAnalytics.framework")

    privacy_src = os.path.join(device, "PrivacyInfo.xcprivacy")
    if os.path.exists(privacy_src):
        with open(privacy_src, "rb") as f:
            privacy = plistlib.load(f)
        os.makedirs(os.path.dirname(IOS_PRIVACY_MANIFEST), exist_ok=True)
        with open(IOS_PRIVACY_MANIFEST, "wb") as f:
            plistlib.dump(privacy, f, fmt=plistlib.FMT_XML)
        log(f"  {os.path.relpath(IOS_PRIVACY_MANIFEST, ROOT_DIR)}")


def install_android(dep, work_dir):
    # The Android SDK comes from maven, so only the version in build.gradle changes.
    with open(ANDROID_GRADLE_FILE, "r", encoding="utf-8") as f:
        gradle = f.read()
    pattern = r"(com\.gameanalytics\.sdk:gameanalytics-android:)[^'\"]+"
    if not re.search(pattern, gradle):
        raise DependencyError(f"gameanalytics-android dependency not found in {ANDROID_GRADLE_FILE}")
    with open(ANDROID_GRADLE_FILE, "w", encoding="utf-8") as f:
        f.write(re.sub(pattern, r"\g<1>" + dep["version"], gradle))
    log(f"  {os.path.relpath(ANDROID_GRADLE_FILE, ROOT_DIR)} -> gameanalytics-android:{dep['version']}")


def install_javascript(dep, work_dir):
    # GA-SDK-JAVASCRIPT has no release assets; the built files are committed in
    # dist/ at each tag. GameAnalytics.min.js already wraps itself in
    # (function(scope){...})(this) and exports gameanalytics/GameAnalytics.
    raw_base = dep["url"].rstrip("/").replace("github.com", "raw.githubusercontent.com")
    js_path = os.path.join(work_dir, "GameAnalytics.min.js")
    download_first([f"{raw_base}/{tag}/dist/GameAnalytics.min.js" for tag in tag_candidates(dep["version"])], js_path)
    os.makedirs(os.path.dirname(JS_FILE), exist_ok=True)
    shutil.copy2(js_path, JS_FILE)
    log(f"  {os.path.relpath(JS_FILE, ROOT_DIR)}")


INSTALLERS = {
    "GA-SDK-CPP": install_cpp,
    "GA-SDK-IOS": install_ios,
    "GA-SDK-ANDROID": install_android,
    "GA-SDK-JAVASCRIPT": install_javascript,
}


def load_dependencies():
    try:
        with open(DEPENDENCIES_FILE, "r", encoding="utf-8") as f:
            dependencies = json.load(f)
    except (OSError, json.JSONDecodeError) as e:
        raise DependencyError(f"cannot read {DEPENDENCIES_FILE}: {e}") from e
    for dep in dependencies:
        if dep.get("name") not in INSTALLERS:
            raise DependencyError(f"unknown dependency '{dep.get('name')}' in dependencies.json")
    return dependencies


def save_dependencies(dependencies):
    with open(DEPENDENCIES_FILE, "w", encoding="utf-8") as f:
        json.dump(dependencies, f, indent=4)
        f.write("\n")


def install_dependencies(only=None):
    """Installs the dependencies in dependencies.json (or just the names in `only`)."""
    for dep in load_dependencies():
        if only and dep["name"] not in only:
            continue
        log(f"--- {dep['name']} {dep['version']} ---")
        # Everything is downloaded and unpacked in a temp dir first, so a failed
        # download never leaves a 404 page or half an archive in the extension.
        with tempfile.TemporaryDirectory(prefix="ga-defold-") as work_dir:
            INSTALLERS[dep["name"]](dep, work_dir)


def check_dependencies(only=None, bump=False):
    dependencies = load_dependencies()
    changed = False
    for dep in dependencies:
        if only and dep["name"] not in only:
            continue
        latest = latest_release(dep)
        status = "up to date" if latest == dep["version"] else f"latest is {latest}"
        log(f"{dep['name']:<20} {dep['version']:<10} {status}")
        if bump and latest != dep["version"]:
            dep["version"] = latest
            changed = True
    if changed:
        save_dependencies(dependencies)
        log(f"updated {os.path.relpath(DEPENDENCIES_FILE, ROOT_DIR)}")


def main(argv):
    parser = argparse.ArgumentParser(description="Install the GameAnalytics SDKs pinned in dependencies.json.")
    parser.add_argument("--only", action="append", choices=sorted(INSTALLERS), help="only process this dependency (repeatable)")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--check", action="store_true", help="only report pinned vs latest release versions")
    group.add_argument("--bump", action="store_true", help="pin the latest release versions, then install")
    args = parser.parse_args(argv)

    try:
        if args.check or args.bump:
            check_dependencies(args.only, bump=args.bump)
        if not args.check:
            install_dependencies(args.only)
    except DependencyError as e:
        print(f"error: {e}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
