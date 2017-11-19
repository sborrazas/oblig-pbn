#!/bin/bash

set -e
set -o pipefail

make build/origin

./build/origin
