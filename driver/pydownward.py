#! /usr/bin/env python

import importlib
pydownward = importlib.import_module('builds.pybindings.search.downward')

def search():
    pydownward.read_task("output.sas")
    task = pydownward.get_root_task()

    infinity = 2000000
    h = pydownward.lmcut(task, pydownward.verbosity.normal, task, True) # pydownward.no_transform() mit task ersetzt
    search = pydownward.astar(h, h, pydownward.null(pydownward.verbosity.normal), pydownward.costtype.normal, infinity, infinity, pydownward.verbosity.normal)

    search.search()
    assert search.found_solution()
    op_id_plan = search.get_plan()
    plan = [task.get_operator_name(op_id.get_index(), False)
        for op_id in op_id_plan]
    print(plan)