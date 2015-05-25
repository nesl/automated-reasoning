#!/bin/bash

make clean
cd ../primitives/
make clean
make
cp libsat.a ../sat_solver/lib/libsat.a
cd ../sat_solver/
make
./sat -in_cnf c17.cnf
