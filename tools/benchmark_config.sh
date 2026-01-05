#!/bin/bash
# Centralized configuration for benchmark scripts

# NUMA configuration
export NUMA_NODE=0

# Thread counts to test
export THREAD_COUNTS="1 2 4 8 12 16 20"

# Python virtual environment path (relative to project root)
export VENV_PATH="venv"
