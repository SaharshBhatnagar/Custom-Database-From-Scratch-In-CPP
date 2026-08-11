#pragma once
#include <string>

enum class MetaCommandResult {
    SUCCESS,
    UNRECOGNIZED
};

enum class PrepareResult {
    SUCCESS,
    UNRECOGNIZED_STATEMENT
};

enum class StatementType {
    INSERT,
    SELECT
};

struct Statement {
    StatementType type;
};

// Function declarations for the compiler
PrepareResult prepare_statement(const std::string& input, Statement* statement);
void execute_statement(Statement* statement);