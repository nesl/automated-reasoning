#!/usr/bin/env python3

import argparse
import os
import subprocess
import fractions
import collections

parser = argparse.ArgumentParser(formatter_class = argparse.RawDescriptionHelpFormatter, description = """

Analyze SAT-solver performance

This script will run a given SAT-solver executable on a directory
of test CNF files.

It expects this directory to contain CNF-formatted SAT problems,
where each problem is specified by a pair of files with matching
prefixes, as follows:

    <file>.cnf -- the actual CNF file that specifies the CNF
                  knowledge base
    <file>.sol -- a single-line file that represents the answer
                  to the problem defined in the first file.
                  It must contain either the string ``SATISFIABLE''
                  or the string ``UNSATISFIABLE''.

###  Needs to be updated: 
###
###  Further, this script expects the SAT-solver executable to print to
###  stdout a single line of text for each problem it is invoked upon.
###  This line should contain two pieces of information separated by a
###  comma (``,''):
###  
###  Firstly, it should contain the solver's answer (``SATISFIABLE''
###  or ``UNSATISFIABLE'')
###  
###  After the comma, it should contain the number of clauses
###  learned while solving the problem
###  
###  Example of expected SAT-solver output: ``UNSATISFIABLE,128''

This script will log its data to stdout.

""")

parser.add_argument(
    "problem_dir",
    metavar = "<problem-directory>",
    help = "The directory of SAT problems"
)

parser.add_argument(
    "--exec",
    help = "SAT-solver executable file (default: ``%(default)s'')",
    metavar = "<path>",
    default = "./sat"
)

parser.add_argument(
    "--reps",
    help = "solve each cnf file %(metavar)s times (default: %(default)s)",
    metavar = "<n>",
    default = 1,
    type = int
)

args = parser.parse_args()

class Metadata:
    pass

def is_number(str):
    try:
        float(str)
        return True
    except ValueError:
        return False

# Parses the metadata dump printed by the solver
def parse_metadatastr(metadatastr):
    metadata = Metadata()

    for line in metadatastr.splitlines():
        if line.startswith("SAT"):
            metadata.answer = "SATISFIABLE"
        if line.startswith("UNSAT"):
            metadata.answer = "UNSATISFIABLE"
        if line.startswith("mem"):
            for field in line.split():
                if field.isdigit():
                    metadata.mem = int(field)
                    break
        if line.startswith("time"):
            for field in line.split():
                if is_number(field):
                    metadata.time = float(field)
                    break

    return metadata

def run_solver_on_basename(solver, basename):
    solverbuf = subprocess.check_output([solver, "-c", "%s.cnf" % basename])
    solverout = solverbuf.decode("utf-8").rstrip()

    metadatastr = solverout.split("########\n")[-1]
    # Solver metadata should begin after a string of 8 sharp signs

    return parse_metadatastr(metadatastr);

def handle_sat_problem(args, filename):
    if filename.endswith(".cnf"):
        basename = ".".join(filename.split(".")[:-1])
        solfilename = os.path.join(args.problem_dir, basename + ".sol")

        solution = open(solfilename, "r").read().rstrip("\n")

        for repidx in range(args.reps):
            results = run_solver_on_basename(args.exec, filename)
            if (results.answer == solution):
                print("   Correct on {:16} (results in {:.5} seconds using {} pages)".format(basename, results.time, results.mem))
            else:
                print("!! Invalid on %s" % basename)
    else:
        raise ValueError("CNF filenames must end with the extension ``.cnf''")

for filename in os.listdir(args.problem_dir):
    if filename.endswith(".cnf"):
        handle_sat_problem(args, filename)
