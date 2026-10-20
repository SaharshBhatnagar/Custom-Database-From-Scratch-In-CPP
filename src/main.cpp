#include <iostream>
#include <string>
#include "repl.h"
#include "compiler.h"
#include "table.h"
#include "benchmark.h"

int main() {

    Table* table = db_open("mydb.db");
    
    print_storage_and_alignment_metrics();

    std::string input_buffer;

    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer.empty()) {
            continue;
        }

        if (input_buffer[0] == '.') {

            if (input_buffer == ".bench") {
                run_database_benchmarks(table);
                continue;
            }

            if (input_buffer == ".metrics") {
                print_storage_and_alignment_metrics();
                continue;
            }
            
            switch (do_meta_command(input_buffer, table)) {
                case (MetaCommandResult::SUCCESS):
                    continue;
                case (MetaCommandResult::UNRECOGNIZED):
                    std::cout << "Unrecognized command '" << input_buffer << "'\n";
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PrepareResult::SUCCESS):
                break;
            case (PrepareResult::UNRECOGNIZED_STATEMENT):
                std::cout << "Unrecognized keyword at start of '" << input_buffer << "'\n";
                continue;
            case (PrepareResult::SYNTAX_ERROR):
                std::cout << "Syntax error. Could not parse statement.\n";
                continue;
        }

        execute_statement(&statement, table);
        std::cout << "Executed.\n";
    }

    return 0;
}