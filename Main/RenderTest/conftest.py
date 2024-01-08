import infra

import pytest
import matplotlib

import sys
import re
import json
from collections import abc


@pytest.hookimpl()
def pytest_sessionstart(session: pytest.Session):
    """
    Called after the Session object has been created and before performing collection and entering the run test loop.
    """
    # Use a non-interactive backend so plot window will not pop out
    matplotlib.use('Agg')

@pytest.hookimpl()
def pytest_sessionfinish(session: pytest.Session, exitstatus: int):
    """
    Called after whole test run finished, right before returning the exit status to the system.
    """

@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item: pytest.Item, call: pytest.CallInfo):
    # Before `yield`: run prior to any other `pytest_runtest_makereport()` hook wrapper
    # On `yield`: run any other `pytest_runtest_makereport()` hook (non-wrapped)
    outcome = yield
    report = outcome.get_result()
    if report.when != 'call':
        return report
    
    # When generating reports, keep in mind that the tests can run in parallel (e.g., using `xdist`).
    # Each report must be generated in a thread-safe and process-safe way.

    # After a test is called, get the test function's containing module and find test cases in it
    module = sys.modules[item.function.__module__]
    cases = []
    for name, value in module.__dict__.items():
        if name.startswith("_") or "case" not in name:
            continue

        if isinstance(value, infra.TestCase):
            cases.append(value)
        elif isinstance(value, abc.Iterable):
            cases.extend([e for e in value if isinstance(e, infra.TestCase)])

    # Extract test ID from square brackets (`item.name` contains "test_func_name[test-id]") and find the case
    called_test_id = re.findall(r'\[(.*?)\]', item.name)[0]
    called_case = [case for case in cases if case.get_name() == called_test_id]
    if not called_case:
        raise ValueError("cannot find corresponding case for test ID <%s>" % called_test_id)
    elif len(called_case) > 1:
        raise ValueError("duplicated cases for the test ID <%s> found (ID collision)" % called_test_id)
    else:
        called_case = called_case[0]

    # Write case info and case result in .json format
    with open((called_case.get_output_dir() / called_case.get_name()).with_suffix(".json"), 'w') as json_file:
        case_info = called_case.to_json_dict()
        case_info["outcome"] = report.outcome
        json_file.write(json.dumps(case_info, indent=4))
