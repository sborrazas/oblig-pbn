#!/bin/bash

set -e
set -o pipefail

make build/origin

./build/origin --address 127.0.0.1 --port 3000
