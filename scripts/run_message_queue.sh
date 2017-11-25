#!/bin/bash

set -e
set -o pipefail

make build/message_queue
make build/modules/origin_server
make build/modules/origin_controller
make build/modules/processor_server
make build/modules/processor_controller

./build/message_queue
