#include <fstream>

#include "../tasks/root_task.h"

void read_task(const std::string &sas_file) {
    std::ifstream task_file(sas_file);
    tasks::read_root_task(task_file);
}