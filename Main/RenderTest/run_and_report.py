import make_report

import pytest

import sys


if __name__ == '__main__':
    # TODO: clean previous test
    # TODO: collect run time
    # TODO: collect system information

    # Ensure we only run tests in the RenderTest directory if no specific path is provided
    pytest_args = ["./RenderTest/"] + sys.argv[1:]

    retcode = pytest.main(pytest_args)

    # TODO: clean previous report
    make_report.write()

    sys.exit(retcode)
