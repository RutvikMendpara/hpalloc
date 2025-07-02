#!/bin/bash

set -e

for i in {1..5}; do
    echo "========== Run #$i =========="

    echo "[Run] fastalloc"
    ./build/allocator_bench --allocator=fastalloc --block-size=64 --count=10000000

    echo ""

    echo "[Run] malloc"
    ./build/allocator_bench --allocator=malloc --block-size=64 --count=10000000

    echo ""
done
