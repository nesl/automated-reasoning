#!/bin/bash

if [[ "$#" -ne "1" ]]
then
	echo "This is a quick and dirty script to generate solutions"
	echo "to SAT problems in a directory using the minisat solver"
	echo ""
	echo "usage: gen_solutions.sh <problems-directory>"
	echo ""
	echo "Solution files will be generated within the provided"
	echo "directory, in a format readable by testsat.py"
	exit
fi

cd "$1"

for file in *.cnf
do
	base="${file%.*}"
	echo Solving "$base"...
	minisat "$base".cnf | tail -n 1 > "$base".sol
done
