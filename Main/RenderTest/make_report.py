from infra import paths
from infra import image
import res

import psutil

import json
import datetime
import os
import platform
import importlib.resources
from pathlib import Path
from typing import Optional


TEST_META_FILENAME = "test_meta.json"
REPORT_WARNINGS_DIRNAME = "_report_warnings"


def _get_img_bytes(img: Path):
    """
    Read image bytes from the given path. If any error occurs, bytes for a fallback image will
    be returned instead.
    """
    try:
        return img.read_bytes()
    except Exception as e:
        print(f"error on loading image {img}: {e}")
        return res.get_error_img_bytes()


def _copy_case_image_to_report(case_output_dir: Path, image_stem: Optional[str]):
    if not image_stem:
        return None

    source_img = (case_output_dir / image_stem).with_suffix(image.Image.default_plot_format)
    report_img = paths.report_output() / source_img.relative_to(paths.test_output())
    report_img.parent.mkdir(parents=True, exist_ok=True)
    report_img.write_bytes(_get_img_bytes(source_img))
    return report_img.relative_to(paths.report_output()).as_posix()


def _copy_case_raw_pfm_to_report(case_output_dir: Path, image_stem: Optional[str]):
    if not image_stem:
        return None

    source_img = (case_output_dir / image_stem).with_suffix(".pfm")
    if not source_img.exists():
        return None
    report_img = paths.report_output() / source_img.relative_to(paths.test_output())
    report_img.parent.mkdir(parents=True, exist_ok=True)
    report_img.write_bytes(source_img.read_bytes())
    return report_img.relative_to(paths.report_output()).as_posix()


def _write_case_report(report_file, case, title_prefix=""):
    outcome = case['outcome']
    if outcome == "passed":
        outcome = outcome.upper()
    else:
        outcome = "!!! " + outcome.upper() + " !!!"
    
    report_file.write("### %sCase %s: *\\<%s\\>*\n\n" % (title_prefix, case['_case_name'], outcome))
    
    case_msg = case['case_msg']
    if case_msg:
        report_file.write("%s\n\n" % case_msg)
    
    report_file.write("Time spent: %d seconds.\n\n" % int(case['secs']))

    case_output_dir = Path(case['_output_dir'])

    # Copy reference image to report output if specified. Sets `ref_img` to a markdown fragment for the image.
    ref_img = _copy_case_image_to_report(case_output_dir, case['plot_ref_image'])
    if ref_img:
        ref_img = "![reference image](%s)" % ref_img
    else:
        ref_img = "(no reference image)"

    # Copy output image to report output if specified. Sets `output_img` to a markdown fragment for the image.
    output_img = _copy_case_image_to_report(case_output_dir, case['plot_output_image'])
    if output_img:
        output_img = "![output image](%s)" % output_img
    else:
        output_img = "(no output image)"

    # Copy debug output image to report output if specified. Sets `debug_output_img` to a markdown fragment for the image.
    debug_output_img = _copy_case_image_to_report(case_output_dir, case['plot_debug_image'])
    if debug_output_img:
        debug_output_img = "![debug output image](%s)" % debug_output_img
    else:
        debug_output_img = "(no debug output image)"

    report_file.write("|  Output  |  Reference  |\n")
    report_file.write("| :------: | :---------: |\n")
    report_file.write("|    %s    |      %s     |\n\n" % (output_img, ref_img))

    report_file.write("#### Debug Output\n\n")
    report_file.write("%s\n\n" % debug_output_img)
    
    debug_msg = case['debug_msg']
    if debug_msg:
        report_file.write("Debug message: %s\n\n" % debug_msg)


def _write_test_report(report_file, test_name, cases):
    report_file.write("## %s\n\n" % test_name)
    if not cases:
        report_file.write("No test case.\n\n")
    else:
        # Assuming each case has the same test description
        report_file.write("%s\n\n" % cases[0]['desc'])

        report_file.write("Contains %d test case(s) (%s).\n\n" % 
            (len(cases), ", ".join([case['_case_name'] for case in cases])))
        
        for i, case in enumerate(cases):
            _write_case_report(report_file, case, title_prefix="(%d/%d) " % (i + 1, len(cases)))


def _write_catalog(report_file, test_to_cases):
    num_failed = 0
    num_passed = 0
    test_to_num_failed = {}
    test_to_num_passed = {}
    test_to_secs = {}
    for test_name, cases in test_to_cases.items():
        test_to_num_failed[test_name] = 0
        test_to_num_passed[test_name] = 0
        test_to_secs[test_name] = 0.0
        for case in cases:
            if case['outcome'] == "passed":
                num_passed += 1
                test_to_num_passed[test_name] += 1
            else:
                num_failed += 1
                test_to_num_failed[test_name] += 1
            
            test_to_secs[test_name] += case['secs']

    report_file.write("## Failed Tests (%d)\n\n" % num_failed)
    if num_failed == 0:
        report_file.write("All tests passed.\n")
    else:
        for test_name, num in test_to_num_failed.items():
            if num == 0:
                continue
            report_file.write("* [(%d) %s (%ds)](#%s)\n" % 
                (num, test_name, int(test_to_secs[test_name]), test_name.replace(" ", "-").lower()))

    report_file.write("\n")

    report_file.write("## Passed Tests (%d)\n\n" % num_passed)
    if num_passed == 0:
        report_file.write("All tests failed.\n")
    else:
        for test_name, num in test_to_num_passed.items():
            if num == 0:
                continue
            report_file.write("* [(%d) %s (%ds)](#%s)\n" % 
                (num, test_name, int(test_to_secs[test_name]), test_name.replace(" ", "-").lower()))
            
    report_file.write("\n")


def _build_test_to_cases(case_infos):
    test_to_cases = {}
    for case in case_infos:
        test_name = case['_test_name']
        if test_name not in test_to_cases:
            test_to_cases[test_name] = []
        test_to_cases[test_name].append(case)
    return test_to_cases


def _meta_json_path():
    return paths.test_output() / TEST_META_FILENAME


def _warning_dir():
    return paths.test_output() / REPORT_WARNINGS_DIRNAME


def _load_report_warnings():
    warning_dir = _warning_dir()
    if not warning_dir.exists():
        return []

    warning_entries = []
    for warning_file in sorted(warning_dir.glob("*.jsonl")):
        with open(warning_file, "r", encoding="utf-8") as stream:
            for line in stream:
                line = line.strip()
                if not line:
                    continue
                warning_entries.append(json.loads(line))
    return warning_entries


def _build_meta(case_infos, total_test_secs):
    num_passed_cases = sum(1 for case in case_infos if case['outcome'] == "passed")
    num_failed_cases = len(case_infos) - num_passed_cases
    ram_info = psutil.virtual_memory()
    disk_info = psutil.disk_usage(paths.engine_build().root)
    return {
        "generation_time_utc": str(datetime.datetime.now(datetime.timezone.utc)),
        "total_cases": len(case_infos),
        "passed_cases": num_passed_cases,
        "failed_cases": num_failed_cases,
        "pass_ratio": (num_passed_cases / len(case_infos) * 100.0) if case_infos else 0.0,
        "total_secs": float(total_test_secs),
        "note": (
            "Note that timing is wall-clock time estimated per-worker. Depending on the settings, "
            "a worker could be a thread or process, etc. If the tests are run in parallel, timing "
            "can be higher than its sequential counterpart."
        ),
        "system": {
            "os": platform.system(),
            "cpu": platform.processor(),
            "cpu_logical": psutil.cpu_count(logical=True),
            "cpu_physical": psutil.cpu_count(logical=False),
            "ram_gib": ram_info.total / (1024 ** 3),
            "disk_total_gib": disk_info.total / (1024 ** 3),
            "disk_free_gib": disk_info.free / (1024 ** 3),
            "disk_used_gib": disk_info.used / (1024 ** 3)
        }
        }


def _write_meta_json(meta):
    meta_path = _meta_json_path()
    meta_path.parent.mkdir(parents=True, exist_ok=True)
    meta_path.write_text(json.dumps(meta, indent=4), encoding="utf-8")


def _read_meta_json():
    meta_path = _meta_json_path()
    if not meta_path.exists():
        return None
    with open(meta_path, 'r', encoding='utf-8') as meta_file:
        return json.loads(meta_file.read())


def _write_html(case_infos, meta, report_warnings):
    paths.report_output().mkdir(parents=True, exist_ok=True)
    report_file_path = paths.report_output() / "report.html"
    report_vendor_dir = paths.report_output() / "vendor"
    report_vendor_dir.mkdir(parents=True, exist_ok=True)
    slider_vendor_ref = importlib.resources.files("res") / "vendor" / "img-comparison-slider.js"
    with importlib.resources.as_file(slider_vendor_ref) as slider_vendor_path:
        (report_vendor_dir / "img-comparison-slider.js").write_bytes(slider_vendor_path.read_bytes())

    test_to_cases = _build_test_to_cases(case_infos)

    tests_payload = []
    for test_name, cases in test_to_cases.items():
        test_secs = sum(case['secs'] for case in cases)
        failed = sum(1 for case in cases if case['outcome'] != "passed")
        passed = len(cases) - failed

        html_cases = []
        for case in cases:
            case_output_dir = Path(case['_output_dir'])
            verifier_modes = []
            verifier_reports = case.get('verifier_reports') or []
            for verifier_report in verifier_reports:
                verifier_modes.append({
                    "verifier": verifier_report.get("verifier") or "Verifier",
                    "passed": bool(verifier_report.get("passed", True)),
                    "message": verifier_report.get("message") or "",
                    "metrics": verifier_report.get("metrics") or {},
                    "plot_output_image": _copy_case_image_to_report(case_output_dir, verifier_report.get("plot_output_image")),
                    "plot_ref_image": _copy_case_image_to_report(case_output_dir, verifier_report.get("plot_ref_image")),
                    "plot_debug_image": _copy_case_image_to_report(case_output_dir, verifier_report.get("plot_debug_image")),
                    "debug_msg": verifier_report.get("debug_msg") or "",
                    "raw_output_image": _copy_case_raw_pfm_to_report(case_output_dir, verifier_report.get("raw_output_image")),
                    "raw_ref_image": _copy_case_raw_pfm_to_report(case_output_dir, verifier_report.get("raw_ref_image"))
                })

            if not verifier_modes:
                verifier_modes.append({
                    "verifier": "Default",
                    "passed": case['outcome'] == "passed",
                    "message": "",
                    "metrics": {},
                    "plot_output_image": _copy_case_image_to_report(case_output_dir, case.get('plot_output_image')),
                    "plot_ref_image": _copy_case_image_to_report(case_output_dir, case.get('plot_ref_image')),
                    "plot_debug_image": _copy_case_image_to_report(case_output_dir, case.get('plot_debug_image')),
                    "debug_msg": case.get('debug_msg') or "",
                    "raw_output_image": _copy_case_raw_pfm_to_report(case_output_dir, case.get('raw_output_image')),
                    "raw_ref_image": _copy_case_raw_pfm_to_report(case_output_dir, case.get('raw_ref_image'))
                })

            html_cases.append({
                "name": case['_case_name'],
                "outcome": case['outcome'],
                "secs": float(case['secs']),
                "case_msg": case.get('case_msg') or "",
                "verifier_modes": verifier_modes
            })

        tests_payload.append({
            "name": test_name,
            "desc": cases[0].get('desc') or "",
            "secs": float(test_secs),
            "failed": failed,
            "passed": passed,
            "cases": html_cases
        })

    payload = {
        "summary": meta,
        "tests": tests_payload,
        "warnings": report_warnings
        }

    # Keep inline JSON from closing the surrounding `<script>` tag.
    payload_json = json.dumps(payload).replace("</", "<\\/")
    report_template = res.get_report_template_text()
    report_file_path.write_text(report_template.replace("__PAYLOAD_JSON__", payload_json), encoding="utf-8")


def _write_markdown(case_infos, meta, report_warnings):
    paths.report_output().mkdir(parents=True, exist_ok=True)
    report_file = open(paths.report_output() / "report.md", 'w')
    report_file.write("# Render Test Report\n\n")
    report_file.write("* Generation Time (UTC): %s\n" % meta["generation_time_utc"])
    report_file.write("* %d cases, %d passed (%f%%)\n" %
        (meta["total_cases"], meta["passed_cases"], meta["pass_ratio"]))
    report_file.write("* %d report warning(s)\n" % len(report_warnings))
    report_file.write("* Total time spent: %d seconds\n" % int(meta["total_secs"]))

    report_file.write("\n")

    report_file.write(meta["note"] + "\n")

    report_file.write("\n")

    system = meta["system"]
    report_file.write("## System Information\n\n")
    report_file.write("* OS: %s\n" % system["os"])
    report_file.write("* CPU: %s (%d logical, %d physical)\n" %
        (system["cpu"], system["cpu_logical"], system["cpu_physical"]))
    report_file.write("* RAM: %.3f GiB installed\n" % system["ram_gib"])
    report_file.write("* Disk: %.2f GiB total, %.2f GiB available, %.2f GiB used\n" %
        (system["disk_total_gib"], system["disk_free_gib"], system["disk_used_gib"]))

    report_file.write("\n")

    test_to_cases = _build_test_to_cases(case_infos)
    _write_catalog(report_file, test_to_cases)

    for test_name, cases in test_to_cases.items():
        _write_test_report(report_file, test_name, cases)

    report_file.write("## Report Warnings\n\n")
    if not report_warnings:
        report_file.write("None.\n\n")
    else:
        for warning_entry in report_warnings:
            report_file.write("* `%s`: %s\n" % (warning_entry["nodeid"], warning_entry["message"]))
        report_file.write("\n")

    report_file.close()


def write(mode):
    output_dir = paths.test_output()

    # Find all test case run results
    case_infos = []
    for dirpath, dirnames, filenames in os.walk(output_dir):
        dirpath = Path(dirpath)
        for filename in filenames:
            if not filename.endswith('.json'):
                continue
            if filename == TEST_META_FILENAME:
                continue
            case_infos.append(dirpath / filename)

    if not case_infos:
        print("No test output found. Report not generated.")
        print("Please run the test first.")
        return

    # Load all into dictionaries (strict).
    for i, case_info in enumerate(case_infos):
        with open(case_info, 'r', encoding='utf-8') as case_file:
            case_infos[i] = json.loads(case_file.read())

    total_test_secs = sum(case['secs'] for case in case_infos)

    # Sort cases based on test name & case name first, so dictionaries created from them are ordered accordingly
    # (`dict` retains insertion order since python 3.7)
    case_infos = sorted(case_infos, key=lambda case: case['_test_name'] + case['_case_name'])
    meta = _build_meta(case_infos, total_test_secs)
    _write_meta_json(meta)
    meta = _read_meta_json() or meta
    report_warnings = _load_report_warnings()

    print("Generating RenderTest report for %d test cases..." % len(case_infos))
    if mode == "both":
        _write_markdown(case_infos, meta, report_warnings)
        _write_html(case_infos, meta, report_warnings)
    elif mode == "markdown":
        _write_markdown(case_infos, meta, report_warnings)
    elif mode == "html":
        _write_html(case_infos, meta, report_warnings)
    else:
        raise ValueError("unsupported report mode: %s (expected: both, markdown, html)" % mode)


if __name__ == '__main__':
    write("both")
