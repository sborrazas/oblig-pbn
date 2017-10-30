#!/bin/bash

set -e
set -o pipefail

make build/fuente

./build/fuente
