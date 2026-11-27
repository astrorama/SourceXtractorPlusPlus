"""Point the CI checkout of Alexandria at the explicitly selected Elements installation."""

import re
import sys
from pathlib import Path
from xml.etree import ElementTree

from dependency_version import dependency_version


def main(cmake_file, manifest):
    project = ElementTree.parse(manifest).find("project[@name='Elements']")
    version = project.get("version", "") if project is not None else ""
    if not re.fullmatch(r"[0-9]+(?:\.[0-9]+)*", version):
        raise SystemExit(f"No Elements version in {manifest}")

    declared = dependency_version(cmake_file, "Elements")
    if declared == version:
        return

    text = cmake_file.read_text(encoding="utf-8")
    pattern = rf"(^[ \t]*elements_project\s*\(\s*Alexandria\s+[^)]+?\bUSE\s+Elements\s+){re.escape(declared)}(?=[\s)])"
    updated, count = re.subn(pattern, rf"\g<1>{version}", text, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"Could not update the Elements dependency in {cmake_file}")
    print(f"Building Alexandria with Elements {version} (declared {declared})")
    cmake_file.write_text(updated, encoding="utf-8")


if __name__ == "__main__":
    main(Path(sys.argv[1]), Path(sys.argv[2]))
