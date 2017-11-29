#!/bin/bash

set -e
set -o pipefail

make build/origin
make build/processor

for i in `seq 0 9`; do
    INTERVAL=$(echo $RANDOM % 10 | bc)
    ./build/origin --name origin0$i --address 127.0.0.1 --port 3000 --interval $INTERVAL &
done

for i in `seq 0 9`; do
    INTERVAL=$(echo $RANDOM % 20 | bc)
    ./build/processor --name procces$i --address 127.0.0.1 --port 4000 --interval $INTERVAL &
done
