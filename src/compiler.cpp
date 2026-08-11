#include <iostream>
#include "compiler.h"

PrepareResult prepare_statement(const std::string& input, Statement* statement) {
    // Basic string matching for our two commands
    if (input.substr(0, 6) == "insert") {
        statement->type = StatementType::INSERT;
        return PrepareResult::SUCCESS;
    }
    if (input.substr(0, 6) == "select") {
        statement->type = StatementType::SELECT;
        return PrepareResult::SUCCESS;
    }

    return PrepareResult::UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch (statement->type) {
        case (StatementType::INSERT):
            std::cout << "Executing an insert operation.\n";
            break;
        case (StatementType::SELECT):
            std::cout << "Executing a select operation.\n";
            break;
    }
}