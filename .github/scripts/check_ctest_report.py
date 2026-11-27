"""Propagate test failures that the Elements make tests target ignores."""

import sys
from pathlib import Path
from xml.etree import ElementTree


def check_report(path):
    # CTest reports missing executables as skipped in JUnit, but lists them as failed here.
    for log in Path(path).parent.glob("Temporary/LastTestsFailed*.log"):
        failures = log.read_text(encoding="utf-8").strip()
        if failures:
            raise SystemExit(f"CTest listed failed tests in {log}:\n{failures}")

    suite = ElementTree.parse(path).getroot()
    cases = suite.findall("testcase")
    if suite.tag != "testsuite" or not cases or len(cases) != int(suite.attrib["tests"]):
        raise SystemExit("CTest produced an empty or incomplete test report")
    if int(suite.attrib["failures"]) or int(suite.get("errors", 0)):
        raise SystemExit(f"CTest reported failures or errors in {path}")
    if not any(case.get("status") == "run" for case in cases):
        raise SystemExit("CTest did not run any tests")
    print(f"CTest reported {len(cases)} tests with no failures or errors")


if __name__ == "__main__":
    check_report(sys.argv[1])
