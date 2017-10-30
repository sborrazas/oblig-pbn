#!/bin/bash

set -e
set -o pipefail

./scripts/run_cola_mensajes.sh
./scripts/run_fuente.sh
./scripts/run_procesador.sh
