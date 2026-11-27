"""Emit a GitHub Actions output for a dependency in an elements_project USE list."""

import re
import sys
from pathlib import Path


VERSION = re.compile(r"[0-9]+(?:\.[0-9]+)*")
TOKEN = re.compile(r'#[^\n]*|"(?:\\.|[^"\\])*"|[()]|[^\s#()"]+')


def project_arguments(text, start):
    """Yield each literal argument and its offset, excluding quotes and line comments."""
    for token in TOKEN.finditer(text, start):
        value = token[0]
        if value == ")":
            break
        if value.startswith("#"):
            continue
        if value.startswith('"'):
            yield value[1:-1], token.start() + 1
        else:
            yield value, token.start()


def dependency_version(cmake_file, dependency):
    """Return the version match in the original CMake text so callers can read or replace it."""
    text = cmake_file.read_text(encoding="utf-8")
    project = re.search(r"^[ \t]*elements_project\s*\(", text, re.MULTILINE | re.IGNORECASE)
    if project:
        arguments = project_arguments(text, project.end())
        for value, _ in arguments:
            if value == "USE":
                break
        for (name, _), (value, start) in zip(arguments, arguments):
            if name in ("DATA", "DESCRIPTION"):
                break
            if name == dependency:
                version = VERSION.fullmatch(text, start, start + len(value))
                if version:
                    return version
                break
    raise SystemExit(f"{cmake_file}: missing literal version for {dependency} in elements_project(... USE ...)")


if __name__ == "__main__":
    print(f"version={dependency_version(Path(sys.argv[1]), sys.argv[2]).group()}")
