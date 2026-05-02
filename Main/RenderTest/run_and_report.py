import make_report

import pytest

import argparse
import sys
import shutil

from infra import paths


def _clean_run_outputs():
    for output_dir in (paths.test_output(), paths.report_output()):
        if output_dir.exists():
            shutil.rmtree(output_dir)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Run RenderTest pytest pipeline and generate reports.")
    parser.add_argument(
        "--mode",
        choices=("both", "markdown", "html"),
        default="both",
        help="Report output mode.")
    parser.add_argument(
        "--report-only",
        action="store_true",
        help="Generate report only from existing test_output JSON files.")
    parser.add_argument(
        "--test-only",
        action="store_true",
        help="Run tests only and skip report generation.")

    args, pytest_user_args = parser.parse_known_args(sys.argv[1:])

    if args.report_only and args.test_only:
        parser.error("--report-only and --test-only cannot be used together")

    retcode = 0

    if not args.report_only:
        _clean_run_outputs()
        # Ensure we only run tests in the RenderTest directory if no specific path is provided
        pytest_args = ["./RenderTest/"] + pytest_user_args
        retcode = pytest.main(pytest_args)

    if not args.test_only:
        make_report.write(mode=args.mode)

    sys.exit(retcode)
