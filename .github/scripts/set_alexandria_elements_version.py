"""Point the CI checkout of Alexandria at the explicitly selected Elements installation."""

import sys
from pathlib import Path
from xml.etree import ElementTree

from dependency_version import VERSION, dependency_version


def main(cmake_file, manifest):
    project = ElementTree.parse(manifest).find("project[@name='Elements']")
    version = project.get("version", "") if project is not None else ""
    if not VERSION.fullmatch(version):
        raise SystemExit(f"No Elements version in {manifest}")

    declared = dependency_version(cmake_file, "Elements")
    if declared.group() == version:
        return

    start, end = declared.span()
    updated = declared.string[:start] + version + declared.string[end:]
    print(f"Building Alexandria with Elements {version} (declared {declared.group()})")
    cmake_file.write_text(updated, encoding="utf-8")


if __name__ == "__main__":
    main(Path(sys.argv[1]), Path(sys.argv[2]))
