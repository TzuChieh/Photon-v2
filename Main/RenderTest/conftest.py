import infra

import pytest
import matplotlib

import sys
import re
import json
import inspect
import warnings
import os
import shutil

try:
    import numpy as np
except ModuleNotFoundError:
    np = None


def _get_module_suites(module):
    suites = []
    for name, value in module.__dict__.items():
        if name.startswith("_"):
            continue
        if isinstance(value, infra.RenderTestSuite):
            suites.append(value)
    return suites


def _to_json_compatible(value):
    """
    Handle values that built-in json cannot serialize.
    """
    if np is not None:
        if isinstance(value, np.generic):
            return _to_json_compatible(value.item())
        if isinstance(value, np.ndarray):
            return [_to_json_compatible(val) for val in value.tolist()]

    if isinstance(value, dict):
        return {str(key): _to_json_compatible(val) for key, val in value.items()}
    if isinstance(value, (list, tuple)):
        return [_to_json_compatible(val) for val in value]
    return value


def _warning_dir():
    return infra.paths.test_output() / "_report_warnings"


def _append_report_warning(nodeid: str, message: str):
    warning_dir = _warning_dir()
    warning_dir.mkdir(parents=True, exist_ok=True)
    worker_name = os.environ.get("PYTEST_XDIST_WORKER", "master")
    warning_file = warning_dir / f"{worker_name}.jsonl"
    payload = {"nodeid": nodeid, "message": message}
    with open(warning_file, "a", encoding="utf-8") as warning_stream:
        warning_stream.write(json.dumps(payload, ensure_ascii=True) + "\n")

@pytest.hookimpl()
def pytest_sessionstart(session: pytest.Session):
    """
    Called after the Session object has been created and before performing collection and entering the run test loop.
    """
    # Use a non-interactive backend so plot window will not pop out
    matplotlib.use('Agg')

    # Prevent stale warnings from previous runs leaking into the next report.
    # In xdist, only controller process should clear this directory.
    if os.environ.get("PYTEST_XDIST_WORKER") is None:
        warning_dir = _warning_dir()
        if warning_dir.exists():
            shutil.rmtree(warning_dir)

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
                    if isinstance(verifier, infra.VisualErrorVerifier) and verifier.get_ref_source().has_image_ref():
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

    # Extract test ID from square brackets (`item.name` contains "test_func_name[test-id]") and find the case.
    called_test_id = re.findall(r'\[(.*?)\]', item.name)[0]

    called_case = [case for case in cases if case.get_name() == called_test_id]
    if not called_case:
        message = "cannot find corresponding case for test ID <%s>; skip json write" % called_test_id
        warnings.warn(message)
        _append_report_warning(item.nodeid, message)
        return report
    elif len(called_case) > 1:
        message = "duplicated cases for the test ID <%s>; skip json write" % called_test_id
        warnings.warn(message)
        _append_report_warning(item.nodeid, message)
        return report
    else:
        called_case = called_case[0]

    # Write case info and case result in .json format
    output_dir = called_case.get_output_dir()
    case_name = called_case.get_name()
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    case_info = called_case.to_json_dict()
    case_info['outcome'] = report.outcome
    case_info['desc'] = inspect.cleandoc(item.function.__doc__)
    case_info['secs'] = call.duration

    case_json_path = (output_dir / case_name).with_suffix('.json')
    case_json_temp_path = (output_dir / (case_name + ".tmp")).with_suffix('.json')
    with open(case_json_temp_path, 'w', encoding='utf-8') as json_file:
        json_file.write(json.dumps(_to_json_compatible(case_info), indent=4))
    case_json_temp_path.replace(case_json_path)
