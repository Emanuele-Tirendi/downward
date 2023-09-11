#! /usr/bin/env python

import importlib
pydownward = importlib.import_module('builds.pybindings.search.downward')

def search():
    pydownward.read_task("output.sas")
    task = pydownward.get_root_task()

    h = pydownward.FFHeuristic(task)
    search = pydownward.EHCSearch("hallo", 0, 100.0, 100, h)
    search.search()
    assert search.found_solution()
    op_id_plan = search.get_plan()
    plan = [task.get_operator_name(op_id.get_index(), False)
        for op_id in op_id_plan]
    print(plan)