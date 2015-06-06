#!/bin/sh

./recompile.sh
./testsat.py --exec ./primitives/sat benchmarks-annotated/sampled/
