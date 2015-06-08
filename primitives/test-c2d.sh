#!/bin/bash

make clean
make
cp libsat.a ../c2D_code/lib/linux/libsat.a
cp include/sat_api.h ../c2D_code/include/sat_api.h

cd ../c2D_code/

make clean
make c2d
#./sat -in_cnf ../Experiments/c17.cnf
./bin/linux/c2D -c ../benchmarks-annotated/sampled/tire-2.cnf -W

cd ../primitives/
