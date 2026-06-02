#!/usr/bin/env bash

# Create Python virtual environment with setup script first
python3 -u "./scripts/setup.py" "$@" --py-env || exit $?

# Run main setup under virtual environment
source "./build/ApplicationEnv/bin/activate"

python3 -u "./scripts/setup.py" "$@"
setup_error=$?

deactivate

if [ $setup_error -ne 0 ]; then
    exit $setup_error
fi
echo "--- Setup Completed ---"
