#pragma once
#include <string>
#include "compiler.h"
#include "table.h"

void print_prompt();
void read_input(std::string& input);
MetaCommandResult do_meta_command(const std::string& input, Table* table);