# Photon Render Test

[TOC]

This directory (`./Main/RenderTest/`) contains the primary end-to-end tests for Photon renderer. **Do not run the test directly in this directory.** After setup (by running the setup script in project root), the test will be installed to the build directory and you can run it from there. As always, use the build directory as the working directory when running tests.

In the build directory, you can run tests using

> pytest ./RenderTest/

or run and produce a markdown report with

> python ./RenderTest/run_and_report.py

## Parallel Test Execution

Most tests already utilizes threading provided by Photon itself. However, tests run by `pytest` is sequential by default. You can install the `pytest-xdist` plugin (`pip install pytest-xdist`) and run tests across multiple CPUs. For example, to run tests using 20 CPUs, you can execute the following command:

> python ./RenderTest/run_and_report.py -n 20

## Run Specified Tests Only

You can use the keyword expressions provided by pytest. For example, to run `test_fullscreen_unit_radiance`, you can use

> python ./RenderTest/run_and_report.py -k unit_radiance

## Writing New Tests

If you do not follow the pattern of existing tests, please keep in mind that the tests can run in parallel (e.g., using `xdist`). All tests must be written in a thread-safe and process-safe way. 
