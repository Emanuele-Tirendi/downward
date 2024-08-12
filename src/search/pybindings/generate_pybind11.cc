// TODO: put generate_pybind11.cc, functions.txt, pydownward.cc, headers_to_include.txt, definitions_for_pybind11_code.h and definitions_for_pybind11_code.cc into a seprarate folder within search

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <filesystem>
#include <experimental/filesystem>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "../plugins/plugin.h"
#include "../plugins/registry.h"

namespace fs = std::filesystem;

void write_head_of_pybind11(std::ofstream& file);
void write_tail_of_pybind11(std::ofstream& file);
std::map<std::string, std::vector<std::string> > get_functions_for_categories();
void write_pybind11_for_category(std::ofstream& file, const plugins::CategoryPlugin* category, std::vector<std:: string>& functions);
void write_pybind11_for_feature(std::ofstream& file, std::shared_ptr<const plugins::Feature> feature);
void write_pybind11_for_other_entities(std::ofstream& file);

int main() {
    std::string path_to_file_string = "../../../src/search/pydownward.cc";
    const char* path_to_file = path_to_file_string.c_str();
    std::ofstream pybind11_file(path_to_file);

    std::vector<const plugins::CategoryPlugin*> categories = plugins::RawRegistry::instance()->get_category_plugins();
    std::vector<std::shared_ptr<const plugins::Feature>> features = plugins::RawRegistry::instance()->construct_registry().get_features();
    write_head_of_pybind11(pybind11_file);
    std::map<std::string, std::vector<std::string> > functions = get_functions_for_categories();
    for (const plugins::CategoryPlugin* category : categories) {
        //TODO: in the future, the if case won't occur anymore.
        if (functions.find(category->get_fully_qualified_name()) == functions.end()) {
            std::vector<std::string> empty_vector = std::vector<std::string>();
            write_pybind11_for_category(pybind11_file, category, empty_vector);
        } else {
            write_pybind11_for_category(pybind11_file, category, functions.at(category->get_fully_qualified_name()));
        }
    }
    for (std::shared_ptr<const plugins::Feature> feature : features) {
        if (feature->get_key() == "astar" ||
            feature->get_key() == "lmcut" ||
            feature->get_key() == "null" ||
            feature->get_key() == "no_transform") {
                write_pybind11_for_feature(pybind11_file, feature);
            }
    }
    write_pybind11_for_other_entities(pybind11_file);
    write_tail_of_pybind11(pybind11_file);
    pybind11_file.close();
}

void write_head_of_pybind11(std::ofstream& file) {
    std::string file_content =
"#include <pybind11/pybind11.h>\n\
#include <pybind11/iostream.h>\n\
#include <pybind11/stl.h>\n\
\n\
#include <memory>\n\
\n\
#include \"pybindings/definitions_for_pybind11_code.h\"\n\
";
    
    std::ifstream file_with_headers("../../../src/search/headers_to_include.txt");
    std::string segment;
    std::map<int, std::string> split_by_dot;
    int i = 0;
    while(std::getline(file_with_headers, segment, '.')) {
        split_by_dot[i] = segment;
        i++;
    }
    file_with_headers.close();
    
    std::vector<std::string> header_files;
    for (int j = 0; j < i - 1; j++) {
        if (split_by_dot.at(j+1).substr(0, 1) == "h") {
            std::string header = split_by_dot.at(j);
            if (j == 0) {
                file_content = file_content + "#include \"" + header + ".h\"\n";
            } else {
                if (split_by_dot.at(j).substr(0, 1) == "h") {
                    file_content = file_content + "#include \"" + header.substr(1, header.length()) + ".h\"\n";
                }
                if (split_by_dot.at(j).substr(0, 2) == "cc") {
                    file_content = file_content + "#include \"" + header.substr(2, header.length()) + ".h\"\n";
                }
                if (split_by_dot.at(j).substr(0, 3) == "cpp") {
                    file_content = file_content + "#include \"" + header.substr(3, header.length()) + ".h\"\n";
                }
            }
        }
    }

    file_content = file_content + 
"\nnamespace py = pybind11;\n\
\n\
PYBIND11_MODULE(downward, m) {\n";

    file << file_content;
}

std::map<std::string, std::vector<std::string> > get_functions_for_categories() {
    std::ifstream functions_file("../../../src/search/pybindings/functions.txt");
    std::string segment;
    std::vector<std::string> classes_with_functions;
    while(std::getline(functions_file, segment, '\n')) {
        classes_with_functions.push_back(segment);
    }
    functions_file.close();
    
    std::map<std::string, std::vector<std::string> > functions;
    for (std::string class_with_function : classes_with_functions) {
        std::stringstream stream(class_with_function);
        // First segment will be the name of the class. Store it separately.
        std::vector<std::string> functions_of_class;
        std::string name_of_class;
        int count = 0;
        while(std::getline(stream, segment, ' ')) {
            if (!count) {
                name_of_class = segment;
                count++;
            } else {
                functions_of_class.push_back(segment);
            }
        }
        functions[name_of_class] = functions_of_class;
    }

    return functions;
}

void write_pybind11_for_category(std::ofstream& file, const plugins::CategoryPlugin* category, std::vector<std::string>& functions) {
    std::string file_content =
"   py::class_<" + category->get_fully_qualified_name() + ", std::shared_ptr<" + category->get_fully_qualified_name() + "> >(m, \"" + category->get_category_name() + "\")\n";
    for (std::string function : functions) {
        file_content += "       .def(\"" + function + "\", &" + category->get_fully_qualified_name() + "::" + function + ")";
    }
    file_content += ";\n\n";
    file << file_content;
}

void write_pybind11_for_feature(std::ofstream& file, std::shared_ptr<const plugins::Feature> feature) {
    //TODO: in the future, this string won't be used anymore
    std::string task_as_constructor_argument = "";
    if (feature->get_key() == "lmcut") {
            task_as_constructor_argument = "std::shared_ptr<AbstractTask>, ";
    }
    
    std::string file_content;
    if (feature->get_fully_qualified_base_name() == feature->get_fully_qualified_constructed_name()) {
        return;
    } else {
        file_content =
        "   py::class_<" + feature->get_fully_qualified_constructed_name() + ", std::shared_ptr<" + feature->get_fully_qualified_constructed_name() + ">, "+ feature->get_fully_qualified_base_name() + ">(m, \"" + feature->get_key() + "\")\n";   
    }
    file_content += "       .def(py::init<" + task_as_constructor_argument + feature->get_constructor_signature() + ">());\n\n";
    file << file_content;
}

void write_pybind11_for_other_entities(std::ofstream& file) {
    std::string file_content =
"   m.def(\"read_task\", &read_task, \"Read the task from sas_file\", py::arg(\"sas_file\")=\"output.sas\");\n\
\n\
    m.def(\"get_root_task\", &tasks::get_root_task, \"Get the root task\");\n\
\n\
    py::class_<OperatorID>(m, \"OperatorID\")\n\
      .def(\"get_index\", &OperatorID::get_index);\n\
\n\
    py::enum_<OperatorCost>(m, \"costtype\")\n\
        .value(\"normal\", OperatorCost::NORMAL)\n\
        .value(\"one\", OperatorCost::ONE)\n\
        .value(\"plusone\", OperatorCost::PLUSONE)\n\
        .value(\"max_operator_cost\", OperatorCost::MAX_OPERATOR_COST)\n\
        .export_values();\n\
\n\
\
    py::enum_<utils::Verbosity>(m, \"verbosity\")\n\
        .value(\"silent\", utils::Verbosity::SILENT)\n\
        .value(\"normal\", utils::Verbosity::NORMAL)\n\
        .value(\"verbose\", utils::Verbosity::VERBOSE)\n\
        .value(\"debug\", utils::Verbosity::DEBUG)\n\
        .export_values();\n\
\n";
    file << file_content;
}

void write_tail_of_pybind11(std::ofstream& file) {
    std::string file_content = "}";
    file << file_content;
}