#include <iostream>
#include <cstring>
#include "compiler.h"
#include "table.h"

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

void execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (StatementType::INSERT): {

            if (table->num_rows >= TABLE_MAX_ROWS) {
                std::cerr << "Error: Table full";
                break;
            }

            void* destination = row_slot(table, table->num_rows);

            serialize_row(&(statement->row_to_insert), destination);

            table->num_rows += 1;

            std::cout << "Executing an insert operation.\n";
            break;
        }

        case (StatementType::SELECT): {
            Row row;

            for (uint32_t i = 0; i < table->num_rows; i+=1) {

                void* source = row_slot(table, i);

                deserialize_row(source, &row);

                std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
            }
            
            std::cout << "Executing a select operation.\n";
            break;
        }
            
    }
}