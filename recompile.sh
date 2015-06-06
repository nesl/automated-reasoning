#!/bin/sh

cd primitives
yes | rm sat
make clean
make sat
cd ..
yes | rm memlog.txt
