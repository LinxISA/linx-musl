#!/usr/bin/env python3

"""Reject Linx musl ACRC sites whose active source block is not SYS."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
SEARCH_ROOTS = (ROOT / "arch/linx64", ROOT / "arch/linx64v5", ROOT / "src")
SUFFIXES = {".c", ".h", ".s", ".S"}
START_RE = re.compile(r"\b(?:c\.)?bstart(?:\.([a-z]+))?\b", re.IGNORECASE)
ACRC_RE = re.compile(r"\bacrc\s+[^\\\"\s]+", re.IGNORECASE)


def source_lines(path: Path):
    for number, raw in enumerate(path.read_text(errors="replace").splitlines(), 1):
        # Inline-assembly C strings are intentionally treated like assembly.
        line = raw.strip()
        if line.startswith(("#", "//", "/*", "*")):
            continue
        yield number, raw.replace('\\n"', "").replace('\\t', " ")


def main() -> None:
    failures: list[str] = []
    for root in SEARCH_ROOTS:
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix not in SUFFIXES:
                continue
            active_block = None
            for number, line in source_lines(path):
                start = START_RE.search(line)
                if start:
                    spelling = start.group(0).lower()
                    suffix = start.group(1)
                    if suffix:
                        active_block = suffix.lower()
                    elif re.search(r"\bbstart\s*\.\s*sys\b", line, re.I):
                        active_block = "sys"
                    elif re.search(r"\bbstart\s*\.\s*(?:std|aux)\b", line, re.I):
                        active_block = re.search(
                            r"\bbstart\s*\.\s*(std|aux)\b", line, re.I
                        ).group(1).lower()
                    elif spelling in {"bstart", "c.bstart"}:
                        active_block = None
                if ACRC_RE.search(line) and active_block != "sys":
                    failures.append(
                        f"{path.relative_to(ROOT)}:{number}: ACRC active block is "
                        f"{active_block or 'unknown'}, expected SYS"
                    )

    if failures:
        raise SystemExit("\n".join(failures))
    print("PASS: every Linx musl ACRC site is enclosed by a SYS block")


if __name__ == "__main__":
    main()
