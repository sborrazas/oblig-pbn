#!/bin/bash

set -e
set -o pipefail

make build/message_queue
make build/modules/origin_server
make build/modules/origin_controller
make build/modules/processor_server
make build/modules/processor_controller

touch build/msg_queue.mem

./build/message_queue --log_file ~/msg_queue.log
