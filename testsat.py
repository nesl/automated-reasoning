#!/usr/bin/env python3

import argparse
import os
import sys
import subprocess
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

Further, this script expects the SAT-solver executable to print
output in the following form:

    The solver may begin by printing any output it wants to stderr
    or stdout.

    Once it is done solving the problem the solver must print
    its solution and some metadata about the solving run.

    This output must be preceeded by a line containing only eight
    sharp marks (``########'') to indicate to the tester script that
    the solver has done its business and will now provide output.

    After the line of eight sharp marks, the solver must print its
    answer, either ``SATISFIABLE'' or ``UNSATISFIABLE''.

    Next, it must print a line starting with ``time'' and containing
    a whitespace-delineated field representing the runtime as a
    decimal number of seconds.

    Next, it must print a line starting with ``mem'' and containing
    a whitespace-delineated field representing the memory usage as
    as an integer number of pages.

    The solver's output should be terminated with a newline.

Example of expected output of the SAT solver:

    debugging something...
    debugging something else...
    anything can go here...
    ########
    UNSATISFIABLE
    time = 0.02254 seconds
    mem = 1982 pages

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

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def is_number(str):
    try:
        float(str)
        return True
    except ValueError:
        return False

# Parses the metadata dump printed by the solver
def parse_metadatastr(metadatastr):
    metadata = {}

    for line in metadatastr.splitlines():
        if line.startswith("SAT"):
            metadata['answer'] = "SATISFIABLE"
        if line.startswith("UNSAT"):
            metadata['answer'] = "UNSATISFIABLE"
        if line.startswith('mem'):
            for field in line.split():
                if field.isdigit():
                    metadata['mem'] = int(field)
                    break
        if line.startswith('time'):
            for field in line.split():
                if is_number(field):
                    metadata['time'] = float(field)
                    break

    if 'answer' not in metadata:
        metadata['parse_successful'] = False
    else:
        metadata['parse_successful'] = True # TODO proper checking

    return metadata

def run_solver_on_file(solver, filepath):
    solverproc = subprocess.Popen(
        [solver, "-c", filepath],
        stderr = subprocess.DEVNULL,
        stdout = subprocess.PIPE,
        universal_newlines = True
    )

    (solverout, _) = solverproc.communicate()

    # Solver metadata should begin after a line of 8 sharp signs
    mdstr = solverout.split("########\n")[-1]

    metadata = parse_metadatastr(mdstr);

    if not metadata['parse_successful']:
        lines = solverout.splitlines()
        if len(lines) > 0:
            print("  {} Wasn't able to parse solver's output for {}".format(
                bcolors.FAIL + "?" + bcolors.ENDC,
                os.path.basename(filepath)
            ))

            print("\n    >> Log of solver output")
            for line in lines:
                print("    >> {}".format(line))
            print("    >> End of solver output")
        else:
            print("  {} Solver returned no output for {}".format(
                bcolors.FAIL + "∅" + bcolors.ENDC,
                os.path.basename(filepath))
            )

    return metadata

def handle_sat_problem(args, filename):
    if filename.endswith(".cnf"):
        basename = ".".join(filename.split(".")[:-1])
        solpath = os.path.join(args.problem_dir, basename + ".sol")
        filepath = os.path.join(args.problem_dir, basename + ".cnf")

        try:
            solution = open(solpath, 'r').read().rstrip("\n")
        except Exception:
            print("!!! Error: found {0}.cnf but no {0}.sol".format(basename))
            return


        for repidx in range(args.reps):
            results = run_solver_on_file(args.exec, filepath)
            if results['parse_successful']:
                if (results['answer'] == solution):
                    print("  {} Correct ({}) on {} ({:.5} seconds using {} pages)".format(
                        bcolors.OKGREEN + bcolors.BOLD + "✓" + bcolors.ENDC,
                        results['answer'],
                        basename,
                        results['time'],
                        results['mem'])
                    )
                else:
                    print("  {} Incorrect (claimed {}, was {}) on {}".format(
                        bcolors.FAIL + "❌" + bcolors.ENDC,
                        results['answer'],
                        solution,
                        basename)
                    )
    else:
        raise ValueError("CNF filenames must end with the extension ``.cnf''")

print("\nRunning SAT solver on files in {}\n".format(args.problem_dir))
for filename in os.listdir(args.problem_dir):
    if filename.endswith(".cnf"):
        handle_sat_problem(args, filename)
