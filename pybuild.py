import os
import subprocess
from colorama import Fore

if os.path.exists("builds/pybindings_code_generation"):
    if not os.path.isdir("builds/pybindings_code_generation"):
        print(Fore.RED + "builds/pybindings_code_generation is not dir" + Fore.WHITE)
        exit()
    else:
        subprocess.run(["rm", "-r", "builds/pybindings_code_generation"])
subprocess.run(["mkdir", "builds/pybindings_code_generation"])
if os.path.exists("builds/pybindings"):
    if not os.path.isdir("builds/pybindings"):
        print(Fore.RED + "builds/pybindings is not dir" + Fore.WHITE)
        exit()
    else:
        subprocess.run(["rm", "-r", "builds/pybindings"])
subprocess.run(["mkdir", "builds/pybindings"])
if os.path.exists("src/search/headers_to_include.txt"):
    if os.path.isfile("src/search/headers_to_include.txt"):
        subprocess.run(["rm", "src/search/headers_to_include.txt"])
subprocess.run(["python3", "build.py", "pybindings_code_generation"])
if os.path.exists("src/search/pydownward.cc"):
    if os.path.isfile("src/search/pydownward.cc"):
        subprocess.run(["rm", "src/search/pydownward.cc"])
os.chdir("builds/pybindings_code_generation/bin")
print(Fore.YELLOW + "downward begin" + Fore.WHITE)
subprocess.run(["./downward"])
print(Fore.YELLOW + "downward end" + Fore.WHITE)
os.chdir("../../..")
subprocess.run(["python3", "build.py", "pybindings"])
subprocess.run(["python3", "fast-downward.py", "pybindings-command-line"])
print(Fore.YELLOW + "test.py" + Fore.WHITE)