#!/usr/bin/env bash

# Create Python virtual environment with setup script first
python3 -u "./scripts/setup.py" "$@" --py-env

# Run main setup under virtual environment
if [ $? -eq 0 ]; then
    source "./build/ApplicationEnv/bin/activate"
    python3 -u "./scripts/setup.py" "$@"
    deactivate
fi

echo "--- Setup Completed ---"
