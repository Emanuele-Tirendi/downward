To install pybind11, use
  apt install python3-pybind11

If you want to add your own feature and added the corresponding source files, 
open the file locatet at src/search/pybindings/functions.txt and type in the
following information: <class_name> <function_name_to_export> [<function_name_to_export> ...]
Overloaded functions aren't supported. Note that you have to use the fully qualified name
for <class_name>, i.e. the class name with all the corresponding namespaces.
For <function_name> you shouldn't use the fully qualified name but only the function name.

To test the current pybind setup, proceed as follows:

Put some output.sas file into the project directory

Either run the following command which delets the necessary files and folders, builds the project and
tests it with the file locatet at driver/pydownward.py, or do it manually. You find all the necessary
commands in pybuild.py
$ ./pybuild.py

Alternatively to the following command invoked in pybuild.py:
$ ./fast-downward.py pybindings-command-line
you can invoke this command when you do it manually.
$ ./fast-downward.py pybindings-dynamic-import