#include <iostream>
#include "repl.h"

void print_prompt() {
    std::cout << "memodb > ";
}

void read_input(std::string& input) {
    std::getline(std::cin, input);
}

MetaCommandResult do_meta_command(const std::string& input, Table* table) {
    if (input == ".exit") {
        std::cout << "Exiting database...\n";
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    return MetaCommandResult::UNRECOGNIZED;
}