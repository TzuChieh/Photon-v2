import pytest
import matplotlib


def pytest_sessionstart(session):
    """
    Called after the Session object has been created and before performing collection and entering the run test loop.
    """
    # Use a non-interactive backend so plot window will not pop out
    matplotlib.use('Agg')

def pytest_sessionfinish(session, exitstatus):
    """
    Called after whole test run finished, right before returning the exit status to the system.
    """
