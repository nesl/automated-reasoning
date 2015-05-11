#!/bin/bash

make clean; make; cp libsat.a ../sat_solver/lib/libsat.a; cd ../sat_solver/; make clean; make; ./sat -in_cnf ../Experiments/c17.cnf; cd ../primitives/
