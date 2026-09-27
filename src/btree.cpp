#include <cstdint>
#include <cstring>
#include <iostream>
#include "btree.h"
#include "table.h"

uint32_t* leaf_node_num_cells(void* node) {
    char* node_bytes = static_cast<char*>(node);
    node_bytes += LEAF_NODE_NUM_CELLS_OFFSET;
    return reinterpret_cast<uint32_t*>(node_bytes);
}

void* leaf_node_cell(void* node, uint32_t cell_num) {
    char* node_bytes = static_cast<char*>(node);
    node_bytes += LEAF_NODE_HEADER_SIZE;
    node_bytes += (LEAF_NODE_CELL_SIZE * cell_num);
    return reinterpret_cast<void*>(node_bytes);
}

uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    void* cell = leaf_node_cell(node, cell_num);
    return reinterpret_cast<uint32_t*>(cell);
}

void* leaf_node_value(void* node, uint32_t cell_num) {
    void* cell = leaf_node_cell(node, cell_num);
    char* cell_bytes = static_cast<char*>(cell);
    cell_bytes += LEAF_NODE_KEY_SIZE;
    return reinterpret_cast<void*>(cell_bytes);
}

void initialize_leaf_node(void* node) {
    uint32_t* num_cells = leaf_node_num_cells(node);
    *num_cells = 0;
}

void leaf_node_insert(void* node, uint32_t cell_num, uint32_t key, Row* value) {
    uint32_t num_cells = *leaf_node_num_cells(node);

    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        std::cerr << "Error: Node full.\n";
        return;
    }

    if (cell_num < num_cells) {
        for (uint32_t i = num_cells; i > cell_num; i--) {
            std::memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) = num_cells + 1;
    *(leaf_node_key(node, cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cell_num));
}

void leaf_node_delete(void* node, uint32_t cell_num) {
    uint32_t num_cells = *leaf_node_num_cells(node);

    if (cell_num >= num_cells) {
        return;
    }

    for (uint32_t i = cell_num; i < num_cells - 1; i++) {
        std::memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i + 1), LEAF_NODE_CELL_SIZE);
    }

    *(leaf_node_num_cells(node)) = num_cells - 1;
}