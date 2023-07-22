#! /usr/bin/env python

import importlib
pydownward = importlib.import_module('builds.pybindings.search.downward')

def search():
    pydownward.read_task("output.sas")
    task = pydownward.get_root_task()

    optionsHeur = pydownward.mimic_options(task)
    h = pydownward.ff(optionsHeur)
    optionsSE = pydownward.Options()
    optionsSE.set_evaluator(h)
    optionsSE.set_evaluator_list("preferred", pydownward.EvaluatorVector())
    optionsSE.set_preferred_usage("preferred_usage", pydownward.PreferredUsage.RANK_PREFERRED_FIRST)
    optionsSE.set_verbosity("Verbosity", pydownward.Verbosity.NORMAL)
    optionsSE.set_unparsed_config("hallo")
    optionsSE.set_operator_cost("cost_type", pydownward.OperatorCost.NORMAL)
    optionsSE.set_double("max_time", 100.0)
    optionsSE.set_int("bound", 100)
    search = pydownward.ehc(optionsSE)
    search.search()
    assert search.found_solution()
    op_id_plan = search.get_plan()
    plan = [task.get_operator_name(op_id.get_index(), False)
        for op_id in op_id_plan]
    print(plan)