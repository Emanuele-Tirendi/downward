#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <functional>
#include <fstream>
#include <vector>

#include "command_line.h"
#include "heuristic.h"
#include "search_engine.h"
#include "task_proxy.h"

#include "search_engines/enforced_hill_climbing_search.h"
#include "heuristics/ff_heuristic.h"
#include "tasks/root_task.h"
#include "task_utils/task_properties.h"
#include "utils/logging.h"
#include "utils/system.h"
#include "utils/timer.h"

namespace py = pybind11;

void read_task(const std::string &sas_file) {
  std::ifstream task_file(sas_file);
  tasks::read_root_task(task_file);
}

void init_ff(py::module_ &m) {
    py::options options;
    options.disable_function_signatures();

    py::class_<ff_heuristic::FFHeuristic, std::shared_ptr<ff_heuristic::FFHeuristic>, Evaluator>(m, "FFHeuristic")
      .def(py::init<std::shared_ptr<AbstractTask>>(), py::arg("task"));
}

void init_ehc(py::module_ &m) {
    py::options options;
    options.disable_function_signatures();

    py::class_<SearchEngine, std::shared_ptr<SearchEngine>>(m, "SearchEngine")
        .def("search", &SearchEngine::search, py::doc("this hopefully has some effect"))
      .def("found_solution", &SearchEngine::found_solution)
      .def("get_plan", &SearchEngine::get_plan);

    py::class_<enforced_hill_climbing_search::EnforcedHillClimbingSearch, std::shared_ptr<enforced_hill_climbing_search::EnforcedHillClimbingSearch>, SearchEngine>(m, "EHCSearch")
        .def(py::init<const std::string &, int, double, int, std::shared_ptr<Evaluator>>());

}

PYBIND11_MODULE(downward, m) {
    m.doc() = "Gabi's pybind11 example plugin"; // Optional module docstring

    py::options options;
    options.disable_function_signatures();

    m.def("read_task", &read_task, "Read the task from sas_file", py::arg("sas_file")="output.sas");

    m.def("get_root_task", &tasks::get_root_task, "Get the root task");

    py::class_<AbstractTask, std::shared_ptr<AbstractTask>>(m, "AbstractTask")
      .def("get_operator_name", &AbstractTask::get_operator_name);

    py::class_<OperatorID>(m, "OperatorID")
      .def("get_index", &OperatorID::get_index);

    py::class_<Evaluator, std::shared_ptr<Evaluator>>(m, "Evaluator");

    std::vector<std::function<void(py::module_ &)>> init_functions = {init_ff, init_ehc};
    for(auto f : init_functions) {
        f(m);
    }
}