#pragma once
#include <string>
#include "row.h"

enum class MetaCommandResult {
    SUCCESS,
    UNRECOGNIZED
};

enum class PrepareResult {
    SUCCESS,
    UNRECOGNIZED_STATEMENT,
    SYNTAX_ERROR
};

enum class StatementType {
    INSERT,
    SELECT
};

struct Statement {
    StatementType type;
    Row row_to_insert;
};

// Function declarations for the compiler
PrepareResult prepare_statement(const std::string& input, Statement* statement);
void execute_statement(Statement* statement);