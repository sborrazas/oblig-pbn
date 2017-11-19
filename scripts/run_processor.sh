#!/bin/bash

set -e
set -o pipefail

make build/processor

./build/processor
