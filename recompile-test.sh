#!/bin/sh

cd primitives
yes | rm sat
make sat
cd ..
./testsat.py --exec ./primitives/sat benchmarks-annotated/sampled/
