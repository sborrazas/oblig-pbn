#!/bin/bash

set -e
set -o pipefail

make build/origin
make build/processor

for i in `seq 1 2`; do
    ./build/origin --name origin0$i --address 127.0.0.1 --port 3000 --interval 4 &
done

for i in `seq 1 2`; do
    ./build/processor --name procces$i --address 127.0.0.1 --port 4000 --interval 2 &
done
