#!/bin/bash
set -e

echo "[Build] Starting build..."
cmake -Bbuild -H. && cmake --build build -j
