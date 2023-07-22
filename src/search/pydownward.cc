#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "evaluator.h"
#include "operator_cost.h"
#include "heuristics/ff_heuristic.h"
#include "search_engines/enforced_hill_climbing_search.h"
#include "tasks/root_task.h"
#include "utils/logging.h"
#include "plugins/any.h"
#include "plugins/registry.h"
#include "plugins/raw_registry.h"
#include "plugins/plugin.h"
#include "plugins/options.h"

namespace py = pybind11;

plugins::RawRegistry *rawRegistry = plugins::RawRegistry::instance();
plugins::Registry registry = rawRegistry->construct_registry();

void read_task(const std::string &sas_file)
{
  std::ifstream task_file(sas_file);
  tasks::read_root_task(task_file);
}

/*struct function {
  function(std::string name, plugins::Any function_pointer, std::string doc) :
    name(name), function_pointer(function_pointer), doc(doc) {}
  std::string name;
  plugins::Any function_pointer;
  std::string doc;
};

std::unordered_map<std::string, std::vector<function>*> categoryFunctions;

void initialize_categoryFunctions() {
  for (const plugins::CategoryPlugin* categoryPlugin : rawRegistry->get_category_plugins()) {
    std::vector<function>* ptr = new std::vector<function>;
    categoryFunctions.insert(categoryPlugin->get_category_name(), ptr);
  }

  // TODO: dont hardcode the key "Evaluator"
  std::vector<function> evaluatorFunctions = categoryFunctions.at("Evaluator");
  // TODO: add documentation
  evaluatorFunctions.push_back(function("search", &SearchEngine::search, ""));
  evaluatorFunctions.push_back(function("found_solution", &SearchEngine::found_solution, ""));
  evaluatorFunctions.push_back(function("get_plan", &SearchEngine::get_plan, ""));
}

void delete_categoryFunctions() {
  for (auto vector_ptr : categoryFunctions) {
    delete vector_ptr;
  }
}*/

/*
//TODO: do we need the constructors of the categories?
template <typename T>
py::class_<T, std::shared_ptr<T>> addConstructor(py::class_<T, std::shared_ptr<T>> pybind_class, std::string category_name) {
  switch(category_name)
  {
    //TODO: add documentation
    case "Evaluator": return pybind_class.def(py::init<plugins::Options&, bool, bool, bool>(), py::arg("opt", "use_for_reporting_minima",
      "use_for_boosting", "use_for_counting_evaluations"), py::doc())
  }
}*/

// TODO: add documentation
template <typename T>
void addCategoryFunctions(const std::string &key, const py::class_<T, std::shared_ptr<T>> &pybind_class)
{
  switch (key)
  {
  case "Evaluator":
    pybind_class = pybind_class.def("search", &SearchEngine::search, "");
    pybind_class = pybind_class.def("found_solution", &SearchEngine::found_solution, "");
    pybind_class = pybind_class.def("get_plan", &SearchEngine::get_plan, "");
    break;
  }
}


template <typename Base, typename Constructed>
void ::add_to_library(plugins::TypedFeature<Base, Constructed>, py::module_ &m) const
{
  py::options options;
  options.disable_function_signatures();
  std::string key = this->get_key();
  py::class_<Constructed, std::shared_ptr<Constructed>, Base>(m, key.c_str())
      // TODO: add documentation
      .def(py::init<const plugins::Options &>(), py::arg("opt"), py::doc(""));
}

template <typename T>
void plugins::TypedCategoryPlugin<T>::add_to_library(py::module_ &m) const
{
  py::options options;
  options.disable_function_signatures();

  py::class_<T, std::shared_ptr<T>> pybind_class(m, get_category_name().c_str());
  // TODO: add documentation
  // addConstructor(pybind_class, this->get_category_name());
  addCategoryFunctions(get_category_name(), pybind_class);
}

PYBIND11_MODULE(pydownward, m)
{

  m.def("read_task", &read_task, "Read the task from sas_file", py::arg("sas_file") = "output.sas");

  m.def("get_root_task", &tasks::get_root_task, "Get the root task");

  for (const plugins::CategoryPlugin *categoryPlugin : rawRegistry->get_category_plugins())
  {
    categoryPlugin->add_to_library(m);
  }

  for (auto feature : registry.get_features())
  {
    feature->add_to_library(m);
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