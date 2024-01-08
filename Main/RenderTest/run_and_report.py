import make_report

import pytest

import sys


# TODO: clean previous test
# TODO: collect run time
# TODO: collect system information
retcode = pytest.main(sys.argv[1:])

# TODO: clean previous report
make_report.write()

sys.exit(retcode)
