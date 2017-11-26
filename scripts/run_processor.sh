#!/bin/bash

set -e
set -o pipefail

make build/processor

./build/processor --name procesad1 --address 127.0.0.1 --port 4000
