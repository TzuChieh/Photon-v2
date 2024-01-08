# Photon Render Test

This directory contains the primary end-to-end tests for Photon renderer. **Do not run the test directly in this directory.** After setup (by running the setup script in project root), the test will be installed to the build directory and you can run it from there. As always, use the build directory as the working directory when running tests.

In the build directory, you can run tests using

> pytest ./RenderTest/

or run and produce a markdown report with

> python ./RenderTest/run_and_report.py
