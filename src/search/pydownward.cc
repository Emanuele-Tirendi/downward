#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "command_line.h"
#include "heuristic.h"
#include "search_engine.h"
#include "task_proxy.h"

#include "search_engines/enforced_hill_climbing_search.h"
#include "evaluator.h"
#include "operator_cost.h"
#include "heuristics/ff_heuristic.h"
#include "search_engines/enforced_hill_climbing_search.h"
#include "tasks/root_task.h"
#include "task_utils/task_properties.h"
#include "utils/logging.h"
#include "utils/system.h"
#include "utils/timer.h"
#include "plugins/any.h"
#include "plugins/registry.h"
#include "plugins/raw_registry.h"
#include "plugins/plugin.h"
#include "plugins/options.h"

namespace py = pybind11;

plugins::RawRegistry *rawRegistry = plugins::RawRegistry::instance();
plugins::Registry registry = rawRegistry->construct_registry();

void read_task(const std::string &sas_file) {
  std::ifstream task_file(sas_file);
  tasks::read_root_task(task_file);
}

PYBIND11_MODULE(downward, m) {
    py::options options;
    options.disable_function_signatures();

    m.def("read_task", &read_task, "Read the task from sas_file", py::arg("sas_file")="output.sas");

    m.def("get_root_task", &tasks::get_root_task, "Get the root task");

    for (const plugins::CategoryPlugin *categoryPlugin : rawRegistry->get_category_plugins()) {
      categoryPlugin->add_to_library(m);
    }

    for (auto feature : registry.get_features()) {
      if (feature->get_key() != "sample_based_potentials" &&
          feature->get_key() != "diverse_potentials" &&
          feature->get_key() != "initial_state_potential" &&
          feature->get_key() != "all_states_potential") {
        feature->add_to_library(m);
      }
    }

    py::class_<OperatorID>(m, "OperatorID")
      .def("get_index", &OperatorID::get_index);

  m.def("mimic_options", &ff_heuristic::mimic_options);

  py::enum_<enforced_hill_climbing_search::PreferredUsage>(m, "PreferredUsage")
      .value("PRUNE_BY_PREFERRED", enforced_hill_climbing_search::PreferredUsage::PRUNE_BY_PREFERRED)
      .value("RANK_PREFERRED_FIRST", enforced_hill_climbing_search::PreferredUsage::RANK_PREFERRED_FIRST);

  py::enum_<utils::Verbosity>(m, "Verbosity")
      .value("SILENT", utils::Verbosity::SILENT)
      .value("NORMAL", utils::Verbosity::NORMAL)
      .value("VERBOSE", utils::Verbosity::VERBOSE)
      .value("DEBUG", utils::Verbosity::DEBUG);

  py::enum_<OperatorCost>(m, "OperatorCost")
      .value("NORMAL", OperatorCost::NORMAL)
      .value("ONE", OperatorCost::ONE)
      .value("PLUSONE", OperatorCost::PLUSONE)
      .value("MAX_OPERATOR_COST", OperatorCost::MAX_OPERATOR_COST);

  py::class_<plugins::Options>(m, "Options")
      .def(py::init<>())
      .def("set_evaluator", &plugins::Options::set<std::shared_ptr<Evaluator>>)
      .def("set_evaluator_list", &plugins::Options::set<std::vector<std::shared_ptr<Evaluator>>>)
      .def("set_preferred_usage", &plugins::Options::set<enforced_hill_climbing_search::PreferredUsage>)
      .def("set_verbosity", &plugins::Options::set<utils::Verbosity>)
      .def("set_unparsed_config", &plugins::Options::set_unparsed_config)
      .def("set_operator_cost", &plugins::Options::set<OperatorCost>)
      .def("set_double", &plugins::Options::set<double>)
      .def("set_int", &plugins::Options::set<int>);

  py::class_<std::vector<Evaluator>>(m, "EvaluatorVector")
      .def(py::init<>());
}
