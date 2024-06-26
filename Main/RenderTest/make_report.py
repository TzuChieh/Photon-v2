from infra import paths
from infra import image
import res

import psutil

import json
import datetime
import platform
from pathlib import Path


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
    ref_img = case['ref']
    if ref_img:
        ref_img = (case_output_dir / ref_img).with_suffix(image.Image.default_plot_format)
        report_ref_img = paths.report_output() / ref_img.relative_to(paths.test_output())
        report_ref_img.parent.mkdir(parents=True, exist_ok=True)
        report_ref_img.write_bytes(_get_img_bytes(ref_img))
        ref_img = "![reference image](%s)" % report_ref_img.relative_to(paths.report_output()).as_posix()
    else:
        ref_img = "(no reference image)"

    # Copy output image to report output if specified. Sets `output_img` to a markdown fragment for the image.
    output_img = case['output']
    if output_img:
        output_img = (case_output_dir / output_img).with_suffix(image.Image.default_plot_format)
        report_output_img = paths.report_output() / output_img.relative_to(paths.test_output())
        report_output_img.parent.mkdir(parents=True, exist_ok=True)
        report_output_img.write_bytes(_get_img_bytes(output_img))
        output_img = "![output image](%s)" % report_output_img.relative_to(paths.report_output()).as_posix()
    else:
        output_img = "(no output image)"

    # Copy debug output image to report output if specified. Sets `debug_output_img` to a markdown fragment for the image.
    debug_output_img = case['debug_output']
    if debug_output_img:
        debug_output_img = (case_output_dir / debug_output_img).with_suffix(image.Image.default_plot_format)
        report_debug_output_img = paths.report_output() / debug_output_img.relative_to(paths.test_output())
        report_debug_output_img.parent.mkdir(parents=True, exist_ok=True)
        report_debug_output_img.write_bytes(_get_img_bytes(debug_output_img))
        debug_output_img = "![debug output image](%s)" % report_debug_output_img.relative_to(paths.report_output()).as_posix()
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


def write():
    output_dir = paths.test_output()

    # Find all test case run results
    case_infos = []
    for dirpath, dirnames, filenames in output_dir.walk():
        for filename in filenames:
            if not filename.endswith('.json'):
                continue
            case_infos.append(dirpath / filename)

    if not case_infos:
        print("No test output found. Report not generated.")
        print("Please run the test first.")
        return

    # Load all into dictionaries
    for i, case_info in enumerate(case_infos):
        with open(case_info, 'r') as case_file:
            case_infos[i] = json.loads(case_file.read())

    num_passed_cases = sum(1 for case in case_infos if case['outcome'] == "passed")
    total_test_secs = sum(case['secs'] for case in case_infos)

    # Sort cases based on test name & case name first, so dictionaries created from them are ordered accordingly
    # (`dict` retains insertion order since python 3.7)
    case_infos = sorted(case_infos, key=lambda case: case['_test_name'] + case['_case_name'])

    # Start generating report in markdown format

    print("Generating RenderTest report for %d test cases..." % len(case_infos))

    paths.report_output().mkdir(parents=True, exist_ok=True)
    report_file = open(paths.report_output() / "report.md", 'w')

    report_file.write("# Render Test Report\n\n")
    report_file.write("* Generation Time (UTC): %s\n" % datetime.datetime.now(datetime.timezone.utc))
    report_file.write("* %d cases, %d passed (%f%%)\n" % 
        (len(case_infos), num_passed_cases, num_passed_cases / len(case_infos) * 100.0))
    report_file.write("* Total time spent: %d seconds\n" % int(total_test_secs))

    report_file.write("\n")

    report_file.write(
        "Note that timing is wall-clock time estimated *per-worker*. Depending on the settings, a worker could be a "
        "thread or process, etc. If the tests are run in parallel, timing can be higher than its sequential counterpart.\n")
    
    report_file.write("\n")

    ram_info = psutil.virtual_memory()
    disk_info = psutil.disk_usage(paths.engine_build().root)
    report_file.write("## System Information\n\n")
    report_file.write("* OS: %s\n" % platform.system())
    report_file.write("* CPU: %s (%d logical, %d physical)\n" % 
        (platform.processor(), psutil.cpu_count(logical=True), psutil.cpu_count(logical=False)))
    report_file.write("* RAM: %.3f GiB installed\n" % (ram_info.total / (1024 ** 3)))
    report_file.write("* Disk: %.2f GiB total, %.2f GiB available, %.2f GiB used\n" % 
        (disk_info.total / (1024 ** 3), disk_info.free / (1024 ** 3), disk_info.used / (1024 ** 3)))

    report_file.write("\n")

    # Organize cases based on the test they belong to
    test_to_cases = {}
    for case in case_infos:
        test_name = case['_test_name']
        if test_name not in test_to_cases:
            test_to_cases[test_name] = []
        test_to_cases[test_name].append(case)

    _write_catalog(report_file, test_to_cases)

    for test_name, cases in test_to_cases.items():
        _write_test_report(report_file, test_name, cases)

    report_file.close()


if __name__ == '__main__':
    write()
