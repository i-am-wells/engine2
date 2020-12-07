import subprocess
from sys import argv
arg_start = 1
print_stdout = False
if argv[arg_start] == "stdout":
    print_stdout = True
    arg_start += 1

res = subprocess.run(argv[arg_start:], stdout=subprocess.PIPE, text=True)
if print_stdout:
    print(res.stdout)
