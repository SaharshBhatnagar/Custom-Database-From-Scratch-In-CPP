#include <iostream>
#include <cstring>
#include "compiler.h"

PrepareResult prepare_statement(const std::string& input, Statement* statement) {

    if (input.length() >= 6 && input.substr(0, 6) == "insert") {
        statement->type = StatementType::INSERT;

        int args_assigned = sscanf(
            input.c_str(), 
            "insert %d %32s %255s", 
            &(statement->row_to_insert.id), 
            statement->row_to_insert.username, 
            statement->row_to_insert.email
        );
        
        
        if (args_assigned < 3) {
            return PrepareResult::SYNTAX_ERROR;
        }
        

        return PrepareResult::SUCCESS;
    }
    if (input.length() >= 6 && input.substr(0, 6) == "select") {
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