#!/bin/bash

set -e
set -o pipefail

make build/procesador

./build/procesador
