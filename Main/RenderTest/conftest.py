import infra

import pytest
import matplotlib

import sys
import re
import json
import inspect


def _get_module_suites(module):
    suites = []
    for name, value in module.__dict__.items():
        if name.startswith("_"):
            continue
        if isinstance(value, infra.RenderTestSuite):
            suites.append(value)
    return suites

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

@pytest.hookimpl()
def pytest_collection_modifyitems(session: pytest.Session, config: pytest.Config, items: list[pytest.Item]):
    """
    Called after collection has been performed. This runs ONLY in the Master process
    before any workers are spawned. We use this to generate all reference plots.
    """
    # Track modules to avoid redundant processing
    processed_modules = set()

    for item in items:
        module = sys.modules[item.function.__module__]
        if module in processed_modules:
            continue

        suites = _get_module_suites(module)
        if not suites:
            processed_modules.add(module)
            continue

        output_dir = infra.paths.test_output() / module.__name__
        
        # Reference plots are written once by the controller process before xdist workers run tests.
        plotted_refs = set()

        for suite in suites:
            for case in suite.get_cases():
                for verifier in case.verifiers:
                    if isinstance(verifier, infra.VisualErrorVerifier) and verifier.has_image_ref():
                        ref_key = verifier.get_ref_key()

                        if ref_key not in plotted_refs:
                            verifier.save_ref_plot(output_dir)
                            plotted_refs.add(ref_key)
        
        processed_modules.add(module)

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

    # After a test is called, get the test function's containing module and find test cases in it.
    # Only direct module-level RenderTestSuite vars are supported.
    module = sys.modules[item.function.__module__]
    suites = _get_module_suites(module)
    cases = []
    for suite in suites:
        cases.extend(suite.get_cases())

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
    output_dir = called_case.get_output_dir()
    case_name = called_case.get_name()
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    with open((output_dir / case_name).with_suffix('.json'), 'w') as json_file:
        case_info = called_case.to_json_dict()
        case_info['outcome'] = report.outcome
        case_info['desc'] = inspect.cleandoc(item.function.__doc__)
        case_info['secs'] = call.duration
        json_file.write(json.dumps(case_info, indent=4))
