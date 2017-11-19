#!/bin/bash

set -e
set -o pipefail

./scripts/run_message_queue.sh
./scripts/run_origin.sh
./scripts/run_processor.sh
