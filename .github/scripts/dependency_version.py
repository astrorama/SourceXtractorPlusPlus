"""Emit a GitHub Actions output for a dependency in an elements_project USE list."""

import re
import shlex
import sys
from pathlib import Path


def dependency_version(cmake_file, dependency):
    text = cmake_file.read_text(encoding="utf-8")
    project = re.search(r"^[ \t]*elements_project\s*\(([^)]*)\)", text, re.MULTILINE | re.IGNORECASE)
    if project:
        arguments = shlex.split(project[1], comments=True)
        if "USE" in arguments:
            dependencies = iter(arguments[arguments.index("USE") + 1:])
            for name, version in zip(dependencies, dependencies):
                if name in ("DATA", "DESCRIPTION"):
                    break
                if name == dependency and re.fullmatch(r"[0-9]+(?:\.[0-9]+)*", version):
                    return version
    raise SystemExit(f"{cmake_file}: missing literal version for {dependency} in elements_project(... USE ...)")


if __name__ == "__main__":
    print(f"version={dependency_version(Path(sys.argv[1]), sys.argv[2])}")
