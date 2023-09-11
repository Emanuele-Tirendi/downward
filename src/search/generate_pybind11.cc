#include <string>
#include <fstream>

int main() {
    std::ofstream pybind11_file("../../../src/search/pydownward.cc");
    std::string file_content =
"#include <pybind11/pybind11.h>\n\
#include <pybind11/stl.h>\n\
#include <functional>\n\
#include <fstream>\n\
#include <vector>\n\
\n\
#include \"command_line.h\"\n\
#include \"heuristic.h\"\n\
#include \"search_engine.h\"\n\
#include \"task_proxy.h\"\n\
\n\
#include \"search_engines/enforced_hill_climbing_search.h\"\n\
#include \"heuristics/ff_heuristic.h\"\n\
#include \"tasks/root_task.h\"\n\
#include \"task_utils/task_properties.h\"\n\
#include \"utils/logging.h\"\n\
#include \"utils/system.h\"\n\
#include \"utils/timer.h\"\n\
\n\
namespace py = pybind11;\n\
\n\
void read_task(const std::string &sas_file) {\n\
  std::ifstream task_file(sas_file);\n\
  tasks::read_root_task(task_file);\n\
}\n\
\n\
void init_ff(py::module_ &m) {\n\
    py::options options;\n\
    options.disable_function_signatures();\n\
\n\
    py::class_<ff_heuristic::FFHeuristic, std::shared_ptr<ff_heuristic::FFHeuristic>, Evaluator>(m, \"FFHeuristic\")\n\
      .def(py::init<std::shared_ptr<AbstractTask>>(), py::arg(\"task\"));\n\
}\n\
\n\
void init_ehc(py::module_ &m) {\n\
    py::options options;\n\
    options.disable_function_signatures();\n\
\n\
    py::class_<SearchEngine, std::shared_ptr<SearchEngine>>(m, \"SearchEngine\")\n\
        .def(\"search\", &SearchEngine::search, py::doc(\"this hopefully has some effect\"))\n\
      .def(\"found_solution\", &SearchEngine::found_solution)\n\
      .def(\"get_plan\", &SearchEngine::get_plan);\n\
\n\
    py::class_<enforced_hill_climbing_search::EnforcedHillClimbingSearch, std::shared_ptr<enforced_hill_climbing_search::EnforcedHillClimbingSearch>, SearchEngine>(m, \"EHCSearch\")\n\
        .def(py::init<const std::string &, int, double, int, std::shared_ptr<Evaluator>>());\n\
\n\
}\n\
\n\
PYBIND11_MODULE(downward, m) {\n\
    m.doc() = \"Gabi's pybind11 example plugin\"; // Optional module docstring\n\
\n\
    py::options options;\n\
    options.disable_function_signatures();\n\
\n\
    m.def(\"read_task\", &read_task, \"Read the task from sas_file\", py::arg(\"sas_file\")=\"output.sas\");\n\
\n\
    m.def(\"get_root_task\", &tasks::get_root_task, \"Get the root task\");\n\
\n\
    py::class_<AbstractTask, std::shared_ptr<AbstractTask>>(m, \"AbstractTask\")\n\
      .def(\"get_operator_name\", &AbstractTask::get_operator_name);\n\
\n\
    py::class_<OperatorID>(m, \"OperatorID\")\n\
      .def(\"get_index\", &OperatorID::get_index);\n\
\n\
    py::class_<Evaluator, std::shared_ptr<Evaluator>>(m, \"Evaluator\");\n\
\n\
    std::vector<std::function<void(py::module_ &)>> init_functions = {init_ff, init_ehc};\n\
    for(auto f : init_functions) {\n\
        f(m);\n\
    }\n\
}";
    pybind11_file << file_content;
}

// #include <fstream>
// #include <iostream>
// #include <sys/stat.h>
// #include <filesystem>
// #include <experimental/filesystem>
// #include <string>
// #include <map>
// #include <vector>
// #include <sstream>

// #include "plugins/plugin.h"

// namespace fs = std::filesystem;

// void write_head_of_pybind11(std::ofstream& file);
// void write_tail_of_pybind11(std::ofstream& file);
// std::map<std::string, std::vector<std::string> > get_functions_for_categories();
// void write_pybind11_for_category(std::ofstream& file, plugins::CategoryPlugin* category, std::vector<std:: string>& functions);
// void write_pybind11_for_feature(std::ofstream& file, plugins::Feature* feature);
// void write_pybind11_for_other_entities(std::ofstream& file);

// int main() {
//     std::string path_to_file_string = "pydownward.cc";
//     const char* path_to_file = path_to_file_string.c_str();
//     std::ofstream pybind11_file(path_to_file);

//     std::vector<const plugins::CategoryPlugin*> categories = plugins::RawRegistry::instance()->get_category_plugins();
//     std::vector<const plugins::Feature*> features = plugins::RawRegistry::instance()->construct_registry()->get_features();
//     write_head_of_pybind11(pybind11_file);
//     std::map<std::string, std::vector<std::string> > functions = get_functions_for_categories();
//     for (CategoryPlugin* category : categories) {
//         //TODO: in the future, the else case won't occur anymore.
//         if (functions.at(category->get_fully_qualified_name()).empty()) {
//             write_pybind11_for_category(pybind11_file, category, functions.at(category->get_fully_qualified_name()));
//         } else {
//             std::vector<std::string> empty_vector = std::vector<std::string>();
//             write_pybind11_for_category(pybind11_file, category, empty_vector);
//         }
//     }
//     for (Feature* feature : features) {
//         write_pybind11_for_feature(pybind11_file, feature);
//     }
//     write_pybind11_for_other_entities(pybind11_file);
//     write_tail_of_pybind11(pybind11_file);
//     pybind11_file.close();
// }

// void write_head_of_pybind11(std::ofstream& file) {
//     std::string file_content =
// "#include <pybind11/pybind11.h>\n\
// #include <pybind11/iostream.h>\n\
// #include <pybind11/stl.h>\n\
// \n\
// #include <memory>\n\
// \n";
    
//     std::string src("../src");
//     std::string ext(".h");
//     for (auto &p : fs::recursive_directory_iterator(src))
//     {
//         if (p.path().extension() == ext) {
//             file_content = file_content + "#include \"../src/" + p.path().stem().string() + ext + "\"\n";
//         }
//     }

//     file_content = file_content + 
// "\nnamespace py = pybind11;\n\
// \n\
// PYBIND11_MODULE(downward, m) {\n";

//     file << file_content;
// }

// std::map<std::string, std::vector<std::string> > get_functions_for_categories() {
//     std::ifstream functions_file("../src/functions.txt");
//     std::string segment;
//     std::vector<std::string> classes_with_functions;
//     while(std::getline(functions_file, segment, '\n')) {
//         classes_with_functions.push_back(segment);
//     }
//     functions_file.close();
    
//     std::map<std::string, std::vector<std::string> > functions;
//     for (std::string class_with_function : classes_with_functions) {
//         std::stringstream stream(class_with_function);
//         // First segment will be the name of the class. Store it separately.
//         std::vector<std::string> functions_of_class;
//         std::string name_of_class;
//         int count = 0;
//         while(std::getline(stream, segment, ' ')) {
//             if (!count) {
//                 name_of_class = segment;
//                 count++;
//             } else {
//                 functions_of_class.push_back(segment);
//             }
//         }
//         functions[name_of_class] = functions_of_class;
//     }

//     return functions;
// }

// void write_pybind11_for_category(std::ofstream& file, CategoryPlugin* category, std::vector<std::string>& functions) {
//     std::string file_content =
// "   py::class_<" + category->get_fully_qualified_name() + ", std::shared_ptr<" + category->get_fully_qualified_name() + "> >(m, \"" + category->get_name() + "\")\n";
//     const std::string fully_qualified_name = category->get_fully_qualified_name();
//     for (std::string function : functions) {
//         file_content += "       .def(\"" + function + "\", &" + category->get_fully_qualified_name() + "::" + function + ")";
//     }
//     file_content += ";\n\n";
//     file << file_content;
// }

// void write_pybind11_for_feature(std::ofstream& file, Feature* feature) {
//     std::string file_content =
// "   py::class_<" + feature->get_fully_qualified_constructed_name() + ", std::shared_ptr<" + feature->get_fully_qualified_constructed_name() + ">, "+ feature->get_fully_qualified_base_name() + ">(m, \"" + feature->get_name() + "\")\n";
//     file_content += "       .def(py::init<" + feature->get_constructor_signature() + ">());\n\n";
//     file << file_content;
// }

// void write_pybind11_for_other_entities(std::ofstream& file) {
//     std::string file_content =
// "   py::class_<nmsp1::UseCategory1, std::shared_ptr<nmsp1::UseCategory1>>(m, \"UseCategory1\")\n\
//         .def(py::init<std::shared_ptr<nmsp1::Category1>>())\n\
//         .def(\"use_category\", &nmsp1::UseCategory1::use_category);\n\
//     \n\
//     py::class_<nmsp2::UseCategory2, std::shared_ptr<nmsp2::UseCategory2>>(m, \"UseCategory2\")\n\
//         .def(py::init<std::shared_ptr<nmsp2::Category2>>())\n\
//         .def(\"use_category\", &nmsp2::UseCategory2::use_category);\n";
//     file << file_content;
// }

// void write_tail_of_pybind11(std::ofstream& file) {
//     std::string file_content = "}";
//     file << file_content;
// }