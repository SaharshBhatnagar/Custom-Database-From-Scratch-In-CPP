#include <iostream>
#include <cstring>
#include "compiler.h"
#include "table.h"
#include "btree.h"

PrepareResult prepare_statement(const std::string& input, Statement* statement) {
    if (input.length() >= 6 && input.substr(0, 6) == "insert") {
        statement->type = StatementType::INSERT;
        int args = sscanf(input.c_str(), "insert %d %32s %255s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
        return (args < 3) ? PrepareResult::SYNTAX_ERROR : PrepareResult::SUCCESS;
    }
    
    if (input.length() >= 6 && input.substr(0, 6) == "select") {
        statement->type = StatementType::SELECT;
        if (input.length() > 7) {
            sscanf(input.c_str(), "select %d", &(statement->target_id));
            statement->is_targeted = true;
        } else {
            statement->is_targeted = false;
        }
        return PrepareResult::SUCCESS;
    }

    if (input.length() >= 6 && input.substr(0, 6) == "delete") {
        statement->type = StatementType::DELETE;
        int args = sscanf(input.c_str(), "delete %d", &(statement->target_id));
        return (args < 1) ? PrepareResult::SYNTAX_ERROR : PrepareResult::SUCCESS;
    }

    if (input.length() >= 6 && input.substr(0, 6) == "update") {
        statement->type = StatementType::UPDATE;
        int args = sscanf(input.c_str(), "update %d %32s %255s", &(statement->target_id), statement->row_to_insert.username, statement->row_to_insert.email);
        statement->row_to_insert.id = statement->target_id;
        return (args < 3) ? PrepareResult::SYNTAX_ERROR : PrepareResult::SUCCESS;
    }

    return PrepareResult::UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (StatementType::INSERT): {
            void* node = get_page(table->pager, table->root_page_num);
            uint32_t num_cells = *(leaf_node_num_cells(node));

            if (num_cells >= LEAF_NODE_MAX_CELLS) {
                std::cerr << "Error: Table full.\n";
                break;
            }

            Cursor* cursor = table_find(table, statement->row_to_insert.id);
            
            if (cursor->cell_num < num_cells && *leaf_node_key(node, cursor->cell_num) == statement->row_to_insert.id) {
                std::cerr << "Error: Duplicate ID.\n";
            } else {
                leaf_node_insert(node, cursor->cell_num, statement->row_to_insert.id, &(statement->row_to_insert));
                std::cout << "Row inserted.\n";
            }
            
            delete cursor;
            break;
        }

        case (StatementType::SELECT): {
            if (statement->is_targeted) {
                Cursor* cursor = table_find(table, statement->target_id);
                void* node = get_page(table->pager, cursor->page_num);
                
                if (cursor->cell_num < *leaf_node_num_cells(node) && *leaf_node_key(node, cursor->cell_num) == statement->target_id) {
                    Row row;
                    deserialize_row(cursor_value(cursor), &row);
                    std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
                } else {
                    std::cout << "Error: ID not found.\n";
                }
                delete cursor;
            } else {
                Cursor* cursor = table_start(table);
                Row row;
                while (!(cursor->end_of_table)) {
                    deserialize_row(cursor_value(cursor), &row);
                    std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
                    cursor_advance(cursor);
                }
                delete cursor;
            }
            break;
        }

        case (StatementType::DELETE): {
            Cursor* cursor = table_find(table, statement->target_id);
            void* node = get_page(table->pager, cursor->page_num);

            if (cursor->cell_num < *leaf_node_num_cells(node) && *leaf_node_key(node, cursor->cell_num) == statement->target_id) {
                leaf_node_delete(node, cursor->cell_num);
                std::cout << "Row deleted.\n";
            } else {
                std::cout << "Error: ID not found.\n";
            }
            delete cursor;
            break;
        }

        case (StatementType::UPDATE): {
            Cursor* cursor = table_find(table, statement->target_id);
            void* node = get_page(table->pager, cursor->page_num);

            if (cursor->cell_num < *leaf_node_num_cells(node) && *leaf_node_key(node, cursor->cell_num) == statement->target_id) {
                serialize_row(&(statement->row_to_insert), leaf_node_value(node, cursor->cell_num));
                std::cout << "Row updated.\n";
            } else {
                std::cout << "Error: ID not found.\n";
            }
            delete cursor;
            break;
        }
    }
}