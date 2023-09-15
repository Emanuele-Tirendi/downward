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


Notes on the interface:

* We hacked the code in some places
  - We made Heuristic::compute_heuristic public. This might be necessary for the
    trampoline classes but have not looked for alternatives.
  - We added a global function get_root_task to access the global task. We have
    to think about how to expose the task.
  - We excluded some features of being exported since their signature doesn't match
    the signature specified in their corresponding feature classes.

* The order of template parameters in py::class_<...> does not matter. Pybind
  will figure out which parameter is which. We should agree on some default order.
  For Heuristic, we use four parameters:
  - Heuristic is the class to expose to Python
  - std::shared_ptr<Heuristic> is the holder that is internally used for
    reference counting, we use shared pointers to make this compatible with our
    other features.
  - Evaluator is the base class
  - HeuristicTrampoline is a class used to handle function calls of virtual
    functions. If we inherit from Heuristic in Python, the trampoline class is
    responsible for forwarding calls to the Python class. See
    https://pybind11.readthedocs.io/en/stable/advanced/classes.html

* Trampoline classes (EvaluatorTrampoline, HeuristicTrampoline) are needed to
  derive classes in Python. They are needed for all classes in the hierarchy. For
  example, if we wanted to allow users to inherit from FFHeuristic, it would also
  need a trampoline class. They have to define all virtual methods that we want to
  make available in Python on every level (FFHeuristicTrampoline would have to
  repeat the definition of compute_heuristic). It is not clear if the trampoline
  for Evaluator is needed as long as no virtual method from Evaluator is in the
  interface. Probably yes, because the constructor counts.

* py::make_iterator can be used to set up an iterable but py::keep_alive is
  required to keep the iterator alive while it is used.
