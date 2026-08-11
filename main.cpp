#include <iostream>
#include <string>
#include "repl.h"
#include "compiler.h"

int main() {
    std::string input_buffer;

    while (true) {
        print_prompt();
        read_input(input_buffer);

        // Ignore empty Enter presses
        if (input_buffer.empty()) {
            continue;
        }

        // Route Meta-Commands
        if (input_buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case (MetaCommandResult::SUCCESS):
                    continue;
                case (MetaCommandResult::UNRECOGNIZED):
                    std::cout << "Unrecognized command '" << input_buffer << "'\n";
                    continue;
            }
        }

        // Route SQL Commands
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PrepareResult::SUCCESS):
                break;
            case (PrepareResult::UNRECOGNIZED_STATEMENT):
                std::cout << "Unrecognized keyword at start of '" << input_buffer << "'\n";
                continue; // Skip execution and restart the loop
        }

        // Execute valid commands
        execute_statement(&statement);
        std::cout << "Executed.\n";
    }

    return 0;
}