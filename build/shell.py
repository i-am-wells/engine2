import subprocess
from sys import argv
print(subprocess.run(argv[1], shell=True, stdout=subprocess.PIPE, text=True).stdout)