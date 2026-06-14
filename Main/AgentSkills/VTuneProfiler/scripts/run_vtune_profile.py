#!/usr/bin/env python3
"""Run Intel VTune Hotspots collection and reports for a native executable."""

from __future__ import annotations

import argparse
import json
import os
import shlex
import subprocess
import sys
from pathlib import Path
from shutil import which


SCRIPT_PATH = Path(__file__).resolve()
SKILL_DIR = SCRIPT_PATH.parents[1]
WORK_DIR = Path.cwd()
SETTINGS_PATH = SKILL_DIR / "local.settings.json"
SCHEMA_PATH = SKILL_DIR / "local.settings.schema.json"
GENERATED_DIR = SKILL_DIR / "Generated"

COMMON_VTUNE_PATHS = [
    Path(r"C:\Program Files (x86)\Intel\oneAPI\vtune\latest\bin64\vtune.exe"),
    Path(r"C:\Program Files\Intel\oneAPI\vtune\latest\bin64\vtune.exe"),
]


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(2)


def work_path(text: str | None) -> Path | None:
    if not text:
        return None
    path = Path(text).expanduser()
    return path if path.is_absolute() else WORK_DIR / path


def load_json(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as f:
        data = json.load(f)
    if not isinstance(data, dict):
        fail(f"{path} must contain a JSON object.")
    return data


def load_settings() -> dict:
    if not SETTINGS_PATH.exists():
        return {}

    try:
        settings = load_json(SETTINGS_PATH)
        schema = load_json(SCHEMA_PATH)
    except OSError as e:
        fail(str(e))
    except json.JSONDecodeError as e:
        fail(f"Invalid JSON: {e}")

    properties = schema.get("properties", {})
    unknown = sorted(set(settings) - set(properties))
    if unknown:
        fail(f"Unknown local settings: {', '.join(unknown)}")

    for key, value in settings.items():
        expected = properties[key].get("type")
        if expected == "string" and not isinstance(value, str):
            fail(f"Local setting {key!r} must be a string.")
        elif expected == "array" and not (
            isinstance(value, list) and all(isinstance(item, str) for item in value)
        ):
            fail(f"Local setting {key!r} must be an array of strings.")

    return settings


def find_vtune(explicit_path: str | None, settings: dict) -> Path:
    candidates = [
        explicit_path,
        settings.get("vtune_exe"),
        os.environ.get("VTUNE_EXE"),
        which("vtune.exe"),
        which("vtune"),
        *COMMON_VTUNE_PATHS,
    ]

    for candidate in candidates:
        path = work_path(str(candidate)) if candidate else None
        if not path:
            continue
        if path.is_file():
            return path

    fail(
        "Could not find vtune.exe. Ask the user for its path, then pass --vtune "
        f"or save vtune_exe in {SETTINGS_PATH}."
    )


def split_target_args(args_text: str | None) -> list[str]:
    if not args_text:
        return []
    try:
        return shlex.split(args_text)
    except ValueError as e:
        fail(f"Could not parse --target-args: {e}")


def run(command: list[str]) -> None:
    print(subprocess.list2cmdline(command))
    subprocess.run(command, check=True)


def add_search_dirs(command: list[str], search_dirs: list[str]) -> None:
    for search_dir in search_dirs:
        command.extend(["-search-dir", search_dir])


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--vtune", help="Path to vtune.exe. If omitted, checks local.settings.json, VTUNE_EXE, PATH, and common install locations.")
    parser.add_argument("--target", help="Executable to profile. Required unless --report-only is used.")
    parser.add_argument("--target-arg", action="append", default=[], help="One target argument. Repeat as needed.")
    parser.add_argument("--target-args", help="Simple target argument string.")
    parser.add_argument("--analysis", default="hotspots", help="VTune analysis type.")
    parser.add_argument("--report", default="hotspots", help="VTune report type.")
    parser.add_argument("--result-dir", help="VTune result directory.")
    parser.add_argument("--report-dir", help="Directory for text and CSV reports.")
    parser.add_argument("--search-dir", action="append", default=[], help="VTune binary/source search directory.")
    parser.add_argument("--report-only", action="store_true", help="Skip collection and generate reports from --result-dir.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    settings = load_settings()
    vtune = find_vtune(args.vtune, settings)

    result_dir = work_path(args.result_dir or settings.get("default_result_dir"))
    if result_dir is None:
        result_dir = GENERATED_DIR / "results" / args.analysis

    report_dir = work_path(args.report_dir or settings.get("default_report_dir"))
    if report_dir is None:
        report_dir = GENERATED_DIR / "reports"

    search_dirs = [*settings.get("search_dirs", []), *args.search_dir]

    if not args.report_only:
        target = work_path(args.target)
        if target is None:
            fail("Missing --target. Ask which existing executable and workload args to profile.")
        if not target.is_file():
            fail(f"Target executable does not exist: {target}")

        result_dir.mkdir(parents=True, exist_ok=True)
        collect = [str(vtune), "-collect", args.analysis, "-result-dir", str(result_dir)]
        add_search_dirs(collect, search_dirs)
        collect.extend(["--", str(target)])
        collect.extend(split_target_args(args.target_args))
        collect.extend(args.target_arg)
        run(collect)

    report_dir.mkdir(parents=True, exist_ok=True)
    for suffix, extra in {
        "txt": [],
        "csv": ["-format", "csv", "-csv-delimiter", "comma"],
    }.items():
        report_path = report_dir / f"{result_dir.name}_{args.report}.{suffix}"
        command = [
            str(vtune),
            "-report",
            args.report,
            "-result-dir",
            str(result_dir),
            "-report-output",
            str(report_path),
            *extra,
        ]
        add_search_dirs(command, search_dirs)
        run(command)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
