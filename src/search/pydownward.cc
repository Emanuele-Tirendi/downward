#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

#include <memory>

#include "pybindings/definitions_for_pybind11_code.h"
#include "task_utils/task_properties.h"
#include "task_utils/successor_generator_internals.h"
#include "task_utils/successor_generator_factory.h"
#include "task_utils/successor_generator.h"
#include "task_utils/causal_graph.h"
#include "tasks/root_task.h"
#include "tasks/delegating_task.h"
#include "tasks/cost_adapted_task.h"
#include "heuristics/lm_cut_landmarks.h"
#include "heuristics/lm_cut_heuristic.h"
#include "search_engines/eager_search.h"
#include "search_engines/search_common.h"
#include "pruning/null_pruning_method.h"
#include "evaluators/sum_evaluator.h"
#include "evaluators/weighted_evaluator.h"
#include "evaluators/combining_evaluator.h"
#include "evaluators/g_evaluator.h"
#include "algorithms/subscriber.h"
#include "algorithms/segmented_vector.h"
#include "algorithms/ordered_set.h"
#include "algorithms/priority_queues.h"
#include "algorithms/int_packer.h"
#include "algorithms/int_hash_set.h"
#include "open_lists/tiebreaking_open_list.h"
#include "open_lists/best_first_open_list.h"
#include "open_lists/alternation_open_list.h"
#include "utils/timer.h"
#include "utils/system_windows.h"
#include "utils/system_unix.h"
#include "utils/system.h"
#include "utils/strings.h"
#include "utils/rng_options.h"
#include "utils/rng.h"
#include "utils/memory.h"
#include "utils/math.h"
#include "utils/markup.h"
#include "utils/logging.h"
#include "utils/language.h"
#include "utils/hash.h"
#include "utils/exceptions.h"
#include "utils/countdown_timer.h"
#include "utils/collections.h"
#include "parser/token_stream.h"
#include "parser/syntax_analyzer.h"
#include "parser/lexical_analyzer.h"
#include "parser/decorated_abstract_syntax_tree.h"
#include "parser/abstract_syntax_tree.h"
#include "plugins/types.h"
#include "plugins/registry_types.h"
#include "plugins/registry.h"
#include "plugins/raw_registry.h"
#include "plugins/plugin_info.h"
#include "plugins/plugin.h"
#include "plugins/options.h"
#include "plugins/doc_printer.h"
#include "plugins/bounds.h"
#include "plugins/any.h"
#include "task_proxy.h"
#include "task_id.h"
#include "state_registry.h"
#include "state_id.h"
#include "search_statistics.h"
#include "search_space.h"
#include "search_progress.h"
#include "search_node_info.h"
#include "search_engine.h"
#include "pruning_method.h"
#include "plan_manager.h"
#include "per_task_information.h"
#include "per_state_information.h"
#include "per_state_bitset.h"
#include "per_state_array.h"
#include "operator_id.h"
#include "operator_cost.h"
#include "open_list_factory.h"
#include "open_list.h"
#include "heuristic.h"
#include "evaluator_cache.h"
#include "evaluator.h"
#include "evaluation_result.h"
#include "evaluation_context.h"
#include "command_line.h"
#include "axioms.h"
#include "abstract_task.h"

namespace py = pybind11;

PYBIND11_MODULE(downward, m) {
   py::class_<SearchEngine, std::shared_ptr<SearchEngine> >(m, "SearchEngine")
       .def("search", &SearchEngine::search)       .def("found_solution", &SearchEngine::found_solution)       .def("get_plan", &SearchEngine::get_plan);

   py::class_<PruningMethod, std::shared_ptr<PruningMethod> >(m, "PruningMethod")
;

   py::class_<OpenListFactory, std::shared_ptr<OpenListFactory> >(m, "OpenList")
;

   py::class_<Evaluator, std::shared_ptr<Evaluator> >(m, "Evaluator")
;

   py::class_<AbstractTask, std::shared_ptr<AbstractTask> >(m, "AbstractTask")
       .def("get_operator_name", &AbstractTask::get_operator_name);

   py::class_<null_pruning_method::NullPruningMethod, std::shared_ptr<null_pruning_method::NullPruningMethod>, PruningMethod>(m, "null")
       .def(py::init<utils::Verbosity>());

   py::class_<eager_search::EagerSearch, std::shared_ptr<eager_search::EagerSearch>, SearchEngine>(m, "astar")
       .def(py::init<std::shared_ptr<Evaluator>, std::shared_ptr<Evaluator>, std::shared_ptr<PruningMethod>, OperatorCost, int, double, utils::Verbosity>());

   py::class_<lm_cut_heuristic::LandmarkCutHeuristic, std::shared_ptr<lm_cut_heuristic::LandmarkCutHeuristic>, Evaluator>(m, "lmcut")
       .def(py::init<std::shared_ptr<AbstractTask>, utils::Verbosity, std::shared_ptr<AbstractTask>, bool>());

   m.def("read_task", &read_task, "Read the task from sas_file", py::arg("sas_file")="output.sas");

    m.def("get_root_task", &tasks::get_root_task, "Get the root task");

    py::class_<OperatorID>(m, "OperatorID")
      .def("get_index", &OperatorID::get_index);

    py::enum_<OperatorCost>(m, "costtype")
        .value("normal", OperatorCost::NORMAL)
        .value("one", OperatorCost::ONE)
        .value("plusone", OperatorCost::PLUSONE)
        .value("max_operator_cost", OperatorCost::MAX_OPERATOR_COST)
        .export_values();

    py::enum_<utils::Verbosity>(m, "verbosity")
        .value("silent", utils::Verbosity::SILENT)
        .value("normal", utils::Verbosity::NORMAL)
        .value("verbose", utils::Verbosity::VERBOSE)
        .value("debug", utils::Verbosity::DEBUG)
        .export_values();

}