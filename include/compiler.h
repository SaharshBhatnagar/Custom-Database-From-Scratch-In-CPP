#pragma once
#include <string>
#include "row.h"
#include "table.h"

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

PrepareResult prepare_statement(const std::string& input, Statement* statement);
void execute_statement(Statement* statement, Table* table);