# Photon Render Test {#main_photon_render_test}

[TOC]

This directory (`./Main/RenderTest/`) contains the primary end-to-end tests for Photon renderer. **Do not run the test directly in this directory.** After setup (by running the setup script in project root), the test will be installed to the build directory and you can run it from there. As always, use the build directory as the working directory when running tests.

Multiple dependencies are needed in order to run render tests. You can install them in one go by executing `pip install -r ./RenderTest/requirements.txt` (in the build directory, presumably under an [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/#create-and-use-virtual-environments)).

In the build directory, you can run tests using

```shell
pytest ./RenderTest/
```

or run and produce a [markdown report](https://github.com/TzuChieh/Photon-v2-TestReport/blob/main/RenderTest/2024_07_05/82d2799/report.md) with

```shell
python ./RenderTest/run_and_report.py
```

### Using Virtual Environment

Running tests under a Python [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/#create-and-use-virtual-environments) can be beneficial, as the packages installed are the exact versions we required and is less likely to interfere with other projects. First, create a virtual environment under the build directory with

```shell
python -m venv ./RenderTestEnv/
```

This will create a directory named `RenderTestEnv` (or the name of your choice). To start using the virtual environment we just created, execute the following command:

```shell
source ./RenderTestEnv/bin/activate
```

After this, you can execute `pip install -r ./RenderTest/requirements.txt` to install all dependencies and run the tests with `python ./RenderTest/run_and_report.py`.

> [!tip]
> Depending on the system, you may need to use `python3` to run the python scripts instead.

## Parallel Test Execution

Most tests already utilizes threading provided by Photon itself. However, tests run by `pytest` is sequential by default. You can install the `pytest-xdist` plugin (`pip install pytest-xdist`) and run tests across multiple CPU cores. For example, to run tests using 20 CPU cores, you can execute the following command:

```shell
python ./RenderTest/run_and_report.py -n 20
```

## Run Specified Tests Only

You can use the keyword expressions provided by pytest. For example, to run `test_fullscreen_unit_radiance`, you can use

```shell
python ./RenderTest/run_and_report.py -k unit_radiance
```

## Writing New Tests

If you do not follow the pattern of existing tests, please keep in mind that the tests can run in parallel (e.g., using `xdist`). All tests must be written in a thread-safe and process-safe way. 
