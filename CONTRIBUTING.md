# Developing the GameAnalytics Defold SDK

[README.md](README.md) is the integration guide for games. This file covers working on the SDK itself.

The repository is both the extension (`gameanalytics/`, which games pull in as a dependency) and a Defold project around it: a sample app in `main/` and the unit tests in `test/`. Open `game.project` in the editor to run either.

### Files to update when the SDK interface changes
* `gameanalytics/src/gameanalytics.cpp`: the lua binding (`gameanalytics.*`)
* `gameanalytics/src/GADefold.cpp`, `GADefold.h`: picks the platform wrapper and hands it the engine (JavaVM, Activity, javascript bridge, and on desktop the HTTP service the SDK sends its requests through)
* `gameanalytics/src/http/`: the desktop HTTP client on the engine's HTTP service
* `test/test_gameanalytics.lua`: the unit tests for the lua binding
* `README.md`: the lua API and the `game.project` settings

`gameanalytics/include/GAWrapper.h`, `GAEnums.h` and `gameanalytics/src/ga/` are vendored from the shared GameAnalytics C++ wrapper. Change them there, not here.

### Supported platforms
arm64-ios (devices and the simulator), armv7-android, arm64-android, x86_64-macos, arm64-macos, x86_64-linux, x86_64-win32 and wasm-web. Defold 1.14 no longer builds 32-bit Windows, armv7 iOS or 32-bit macOS.

### Game keys
The repository ships without GameAnalytics keys. To run the sample or the end-to-end check, put the keys of a game of yours in the `[gameanalytics]` section of `game.project` (`game_key_<platform>`, `secret_key_<platform>`), and don't commit them.

## Updating the GameAnalytics SDK dependencies

The native SDKs are downloaded from their GitHub releases at the versions pinned in `dependencies.json`:

| Dependency | Installed into |
|---|---|
| [gameanalytics-sdk-cpp](https://github.com/GameAnalytics/gameanalytics-sdk-cpp/releases) (`static-no-deps`) | `gameanalytics/lib/{osx,x86_64-linux,x86_64-win32}`, `gameanalytics/include/GameAnalytics` |
| [GA-SDK-IOS](https://github.com/GameAnalytics/GA-SDK-IOS/releases) | `gameanalytics/lib/{arm64-ios,arm64_sim-ios}/GameAnalytics.framework`, `gameanalytics/manifests/ios/PrivacyInfo.xcprivacy` |
| [GA-SDK-ANDROID](https://github.com/GameAnalytics/GA-SDK-ANDROID/releases) | maven version in `gameanalytics/manifests/android/build.gradle` |
| [GA-SDK-JAVASCRIPT](https://github.com/GameAnalytics/GA-SDK-JAVASCRIPT/releases) | `gameanalytics/res/web/GameAnalytics.js` |

```
python3 download_dependencies.py                      # install the pinned versions
python3 download_dependencies.py --only GA-SDK-IOS    # install one dependency
python3 download_dependencies.py --check              # compare pinned versions with the latest releases
python3 download_dependencies.py --bump               # pin the latest releases, then install
```

Set `GITHUB_TOKEN` if `--check`/`--bump` hit the GitHub API rate limit.

### Desktop HTTP
The desktop libraries are the C++ SDK's `static-no-deps` builds, which have no HTTP client of their own: `gameanalytics/src/http/GAHttpClientDefold.cpp` sends the SDK's requests through the engine's HTTP service (`dmsdk/dlib/http.h`). So the extension needs no curl or OpenSSL, and requests use the engine's TLS and proxy handling.

### Windows
Defold links the static MSVC runtime (`/MT`). The released Windows libraries use the DLL runtime (`/MD`) and fail to link with a `RuntimeLibrary` mismatch. `download_dependencies.py` takes Windows from the `ga-sdk-static-mt` package once a release has one (gameanalytics-sdk-cpp branch `feature/windows-static-crt`), and leaves `gameanalytics/lib/x86_64-win32` alone until then.

The vendored `GameAnalytics.lib` is the `windows-2022-cl-Release-static-no_deps-mt` artifact of that branch's CI ([run 38399354704](https://github.com/GameAnalytics/gameanalytics-sdk-cpp/actions/runs/38399354704)): the v5.4.1 sources, built with `/MT` and asking the linker for lowercase Windows libraries (`wininet.lib`, `ws2_32.lib`), since the build server's file system is case sensitive.

### Android
`gameanalytics-android` is used without its `play-services-appset` dependency. The Defold build server fails to download the Google and androidx libraries it pulls in (the requests time out), so the build failed for every Android target. Without it:

* the App Set ID (`android_app_set_id`) is not sent; the SDK logs a caught `NoClassDefFoundError` and carries on
* error reporting (`gameanalytics.enable_error_reporting = 1`, off by default) needs `androidx.core`, which the game then has to get from another extension

A game that already has `play-services-appset` or `androidx.core` through another extension (ads, Firebase, ...) gets both back.

### Linux
The released Linux library is built against glibc 2.38+, which the Defold build server doesn't have. `gameanalytics/src/ga/desktop/GALinuxCompat.cpp` provides the missing `__isoc23_*` functions.

## Unit tests and CI

The unit tests in `test/` use [deftest](https://github.com/britzl/deftest) and cover the lua binding of the extension. `test/test.settings` swaps the bootstrap collection for the test runner, so the sample in `main/` stays the default.

After the unit tests, the same run does an end-to-end check against the GameAnalytics servers (`test/e2e.lua`): it initializes, waits up to 30s for the remote configs, sends one event of each type and waits for the event queue to send them. The run exits with 0 (passed), 1 (a unit test failed), 2 (the end-to-end check timed out) or 3 (it failed). Without game keys in `game.project` the end-to-end check is skipped and the run passes on the unit tests alone.

`.github/workflows/ci.yml` runs on every push to master and every pull request:

* **Unit tests:** bundles the tests for x86_64-linux and x86_64-win32 (headless) and wasm-web, and runs them. The keys for the end-to-end check come from the `GA_GAME_KEY` and `GA_SECRET_KEY` repository secrets, written to `ci-keys.settings` before the build; CI checks the SDK logged `Event queue: N events sent`, and retries a timeout up to 3 times. Pull requests from forks don't get the secrets, so they only run the unit tests. The web build runs in headless Chromium and `.github/scripts/run_web_tests.py` reads the results from the console.
* **Sample:** bundles the sample for x86_64-linux, x86_64-win32, macOS, wasm-web and Android, and uploads each bundle as an artifact. For arm64-ios it only builds the engine, since bundling needs signing.

The runner downloads `bob.jar` (about 230 MB) for the Defold version in `DEFOLD_VERSION` and caches it between runs. Native code is compiled on the Defold build server (`BUILD_SERVER`).

To run the tests locally (needs OpenJDK 25), use your own platform, e.g. `arm64-macos` on Apple Silicon:

```
java -jar bob.jar --platform arm64-macos --variant headless --archive --settings test/test.settings --bundle-output dist/test resolve build bundle
```

Then run the executable in `dist/test`. You can also open the project in the editor, set the bootstrap collection to `/test/test.collection`, and run it.

`test/test.script` replaces `os.exit` with `sys.exit` for deftest: `os.exit` kills the process without finalizing the extensions, and the engine crashes while the GameAnalytics SDK threads are still running.

## Releasing a new version

1. Pin the GameAnalytics SDK versions to ship in `dependencies.json` (`python3 download_dependencies.py --check` lists newer releases, `--bump` pins and installs them) and commit the libraries.
2. Set `#define VERSION` in `gameanalytics/src/gameanalytics.cpp`. It is sent with every event as the SDK version.
3. Add the entry under `<!--(CHANGELOG_TOP)-->` in `CHANGELOG.md`.
4. Merge to `master` and tag the commit with the version, e.g. `5.0.0`. Games reference the tag's archive in their `game.project` dependencies.
