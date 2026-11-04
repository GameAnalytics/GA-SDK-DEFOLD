#!/usr/bin/env python3
"""
Runs the unit and end-to-end tests in a wasm-web bundle in headless Chromium.

A web build can't hand an exit code back, so this serves the bundle, loads it
and reads what the test runner (test/test.script) prints to the console:

    DEBUG:SCRIPT: 28 tests 9 passed 45 assertions 0 failed 0 errors ...
    DEBUG:SCRIPT: E2E: PASS - initialized, remote configs received, ...

and checks the javascript SDK logged "Event queue: N events sent".

Exit codes match the native test runs: 0 passed, 1 a unit test failed,
2 the end-to-end check timed out on every attempt, 3 it failed. A timeout
(including a run that never logged "events sent") is retried with a fresh
page load; nothing else is. A run without GameAnalytics keys skips the
end-to-end check ("E2E: SKIPPED") and passes on the unit tests alone.

Usage: run_web_tests.py <bundle dir> [timeout seconds per attempt] [attempts]
Needs: pip install playwright && playwright install chromium
"""

import functools
import http.server
import os
import re
import sys
import threading
import time

from playwright.sync_api import sync_playwright

SUMMARY = re.compile(r"(\d+) tests (\d+) passed (\d+) assertions (\d+) failed (\d+) errors")
E2E_RESULT = re.compile(r"E2E: (PASS|TIMEOUT|FAIL|SKIPPED) - (.*)")
EVENTS_SENT = re.compile(r"Event queue: \d+ events sent")

EXIT_OK, EXIT_UNIT_FAILED, EXIT_E2E_TIMEOUT, EXIT_E2E_FAILED = 0, 1, 2, 3


def find_index(bundle_dir):
    for root, _, files in os.walk(bundle_dir):
        if "index.html" in files:
            return root
    sys.exit(f"no index.html in {bundle_dir}")


class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, *args):
        pass


def serve(directory):
    handler = functools.partial(QuietHandler, directory=directory)
    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    return server


def run_once(browser, url, timeout):
    """Loads the page once and returns its exit code."""
    state = {"summary": None, "e2e": None, "events_sent": False}
    page_errors = []

    def on_console(message):
        text = message.text
        print(text, flush=True)
        if (match := SUMMARY.search(text)):
            state["summary"] = [int(group) for group in match.groups()]
        if (match := E2E_RESULT.search(text)):
            state["e2e"] = match.group(1)
        if EVENTS_SENT.search(text):
            state["events_sent"] = True

    page = browser.new_page()
    page.on("console", on_console)
    page.on("pageerror", lambda error: page_errors.append(str(error)))
    page.goto(url)

    deadline = time.time() + timeout
    while time.time() < deadline:
        summary = state["summary"]
        if summary and (summary[0] == 0 or summary[3] or summary[4]):
            break  # unit tests failed, no end-to-end phase follows
        if state["e2e"]:
            break
        page.wait_for_timeout(500)
    # deftest prints its reports after the summary
    page.wait_for_timeout(2000)
    page.close()

    for error in page_errors:
        print(f"page error: {error}", file=sys.stderr)

    summary = state["summary"]
    if summary is None:
        print(f"no test summary within {timeout:.0f}s", file=sys.stderr)
        return EXIT_UNIT_FAILED
    tests, _, _, failed, errors = summary
    if tests == 0 or failed or errors:
        print(f"{failed} failed, {errors} errors out of {tests} tests", file=sys.stderr)
        return EXIT_UNIT_FAILED

    if state["e2e"] == "SKIPPED":
        return EXIT_OK
    if state["e2e"] == "FAIL":
        return EXIT_E2E_FAILED
    if state["e2e"] != "PASS":
        print(f"no end-to-end result within {timeout:.0f}s", file=sys.stderr)
        return EXIT_E2E_TIMEOUT
    if not state["events_sent"]:
        print("the SDK never logged 'events sent'", file=sys.stderr)
        return EXIT_E2E_TIMEOUT
    return EXIT_OK


def main(argv):
    if not argv:
        sys.exit(__doc__)
    timeout = float(argv[1]) if len(argv) > 1 else 180
    attempts = int(argv[2]) if len(argv) > 2 else 3
    server = serve(find_index(argv[0]))
    url = f"http://127.0.0.1:{server.server_address[1]}/index.html"

    code = EXIT_E2E_TIMEOUT
    with sync_playwright() as playwright:
        # Software WebGL, the runners have no GPU.
        browser = playwright.chromium.launch(args=["--enable-unsafe-swiftshader", "--use-angle=swiftshader", "--ignore-gpu-blocklist"])
        for attempt in range(1, attempts + 1):
            code = run_once(browser, url, timeout)
            if code != EXIT_E2E_TIMEOUT:
                break
            print(f"end-to-end check timed out (attempt {attempt} of {attempts})", file=sys.stderr)
            if attempt < attempts:
                time.sleep(10)
        browser.close()
    server.shutdown()
    return code


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
