from infra import paths
from infra import image

import json
import datetime
from pathlib import Path


def _write_case_report(report_file, case, title_prefix=""):
    outcome = case['outcome']
    if outcome == "passed":
        outcome = outcome.upper()
    else:
        outcome = "!!! " + outcome.upper() + " !!!"
    
    report_file.write("### %sCase %s: *\\<%s\\>*\n\n" % (title_prefix, case['_case_name'], outcome))
    
    case_output_dir = Path(case['_output_dir'])

    # Copy reference image to report output if specified. Sets `ref_img` to a markdown fragment for the image.
    ref_img = case['ref']
    if ref_img:
        ref_img = (case_output_dir / ref_img).with_suffix(image.Image.default_plot_format)
        report_ref_img = paths.report_output() / ref_img.relative_to(paths.test_output())
        report_ref_img.parent.mkdir(parents=True, exist_ok=True)
        report_ref_img.write_bytes(ref_img.read_bytes())
        ref_img = "![reference image](%s)" % report_ref_img.relative_to(paths.report_output())
    else:
        ref_img = "(no reference image)"

    # Copy output image to report output if specified. Sets `output_img` to a markdown fragment for the image.
    output_img = case['output']
    if output_img:
        output_img = (case_output_dir / output_img).with_suffix(image.Image.default_plot_format)
        report_output_img = paths.report_output() / output_img.relative_to(paths.test_output())
        report_output_img.parent.mkdir(parents=True, exist_ok=True)
        report_output_img.write_bytes(output_img.read_bytes())
        output_img = "![output image](%s)" % report_output_img.relative_to(paths.report_output())
    else:
        output_img = "(no output image)"

    # Copy debug output image to report output if specified. Sets `debug_output_img` to a markdown fragment for the image.
    debug_output_img = case['debug_output']
    if debug_output_img:
        debug_output_img = (case_output_dir / debug_output_img).with_suffix(image.Image.default_plot_format)
        report_debug_output_img = paths.report_output() / debug_output_img.relative_to(paths.test_output())
        report_debug_output_img.parent.mkdir(parents=True, exist_ok=True)
        report_debug_output_img.write_bytes(debug_output_img.read_bytes())
        debug_output_img = "![debug output image](%s)" % report_debug_output_img.relative_to(paths.report_output())
    else:
        debug_output_img = "(no debug output image)"

    report_file.write("|  Output  |  Reference  |\n")
    report_file.write("| :------: | :---------: |\n")
    report_file.write("|    %s    |      %s     |\n\n" % (output_img, ref_img))

    report_file.write("#### Debug Output\n\n")
    report_file.write("%s\n\n" % debug_output_img)


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


def write():
    output_dir = paths.test_output()

    # Find all test case run results
    case_infos = []
    for dirpath, dirnames, filenames in output_dir.walk():
        for filename in filenames:
            if not filename.endswith('.json'):
                continue
            case_infos.append(dirpath / filename)

    # Load all into dictionaries
    for i, case_info in enumerate(case_infos):
        with open(case_info, 'r') as case_file:
            case_infos[i] = json.loads(case_file.read())

    num_passed_cases = sum(1 for case in case_infos if case['outcome'] == "passed")

    # Start generating report in markdown format

    print("Generating RenderTest report for %d test cases..." % len(case_infos))

    paths.report_output().mkdir(parents=True, exist_ok=True)
    report_file = open(paths.report_output() / "report.md", 'w')

    report_file.write("# Render Test Report\n\n")
    report_file.write("* Generation Time (UTC): %s\n" % datetime.datetime.now(datetime.timezone.utc))
    report_file.write("* %d cases, %d passed (%f%%)\n" % 
        (len(case_infos), num_passed_cases, num_passed_cases / len(case_infos) * 100.0))
    report_file.write("\n")

    test_to_cases = {}
    for case in case_infos:
        test_name = case['_test_name']
        if test_name not in test_to_cases:
            test_to_cases[test_name] = []
        test_to_cases[test_name].append(case)

    for test_name, cases in test_to_cases.items():
        _write_test_report(report_file, test_name, sorted(cases, key=lambda case: case['_case_name']))

    report_file.close()
