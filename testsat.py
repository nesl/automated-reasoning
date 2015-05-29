#!/usr/bin/env python3

import argparse
import os
import subprocess

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

Further, this script expects the SAT-solver executable to print to
stdout a single line of text for each problem it is invoked upon.
This line should contain two pieces of information separated by a
comma (``,''):

Firstly, it should contain the solver's answer (``SATISFIABLE''
or ``UNSATISFIABLE'')

After the comma, it should contain the number of clauses
learned while solving the problem

Example of expected SAT-solver output: ``UNSATISFIABLE,128''

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

def run_solver_on_basename(solver, basename):
    solver_buffer = subprocess.check_output([solver, "-c", "%s.cnf" % basename])
    solver_output = solver_buffer.decode("utf-8").rstrip()

    solver_answer = solver_output.split(",")[0]
    solver_metric = int(solver_output.split(",")[1])

    return solver_answer, solver_metric

def handle_sat_problem(args, filename):
    if filename.endswith(".cnf"):
        basename = ".".join(filename.split(".")[:-1])
        solfilename = os.path.join(args.problem_dir, basename + ".sol")

        solution = open(solfilename, "r").read().rstrip("\n")

        for repidx in range(args.reps):
            answer, metric = run_solver_on_basename(args.exec, filename)
            if (answer == solution):
                print("   Correct on %s" % basename)
            else:
                print("!! Invalid on %s" % basename)
    else:
        raise ValueError("CNF filenames must end with the extension ``.cnf''")

for filename in os.listdir(args.problem_dir):
    if filename.endswith(".cnf"):
        handle_sat_problem(args, filename)
