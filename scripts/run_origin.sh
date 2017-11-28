#!/bin/bash

set -e
set -o pipefail

make build/origin

./build/origin --name origin01 --address 127.0.0.1 --port 3000 --interval 4
