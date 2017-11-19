#!/bin/bash

set -e
set -o pipefail

make build/message_queue

./build/message_queue
