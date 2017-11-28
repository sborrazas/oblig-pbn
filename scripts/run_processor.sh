#!/bin/bash

set -e
set -o pipefail

make build/processor

./build/processor --name process1 --address 127.0.0.1 --port 4000 --interval 2
