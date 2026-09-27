#pragma once
#include "row.h"
#include <string>

enum class StatementType {
    INSERT,
    SELECT,
    DELETE,
    UPDATE
};

struct Statement {
    StatementType type;
    Row row_to_insert;
    uint32_t target_id;
    bool is_targeted;   
};

enum class PrepareResult {
    SUCCESS,
    SYNTAX_ERROR,
    UNRECOGNIZED_STATEMENT
};

PrepareResult prepare_statement(const std::string& input, Statement* statement);

// Forward declaration so the compiler knows Table exists
struct Table; 
void execute_statement(Statement* statement, Table* table);