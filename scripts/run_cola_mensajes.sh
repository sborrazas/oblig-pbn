#!/bin/bash

set -e
set -o pipefail

make build/cola_mensajes

./build/cola_mensajes
