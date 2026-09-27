#include "table.h"
#include "btree.h"
#include <cstring>

void serialize_row(Row* source, void* destination) {
    char* dest_ptr = static_cast<char*>(destination);
    std::memcpy(dest_ptr, &(source->id), ID_SIZE);
    std::memcpy(dest_ptr + ID_SIZE, (source->username), USERNAME_SIZE);
    std::memcpy(dest_ptr + ID_SIZE + USERNAME_SIZE, (source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    char* source_ptr = static_cast<char*>(source);
    std::memcpy(&(destination->id), source_ptr, ID_SIZE);
    std::memcpy((destination->username), source_ptr + ID_SIZE, USERNAME_SIZE);
    std::memcpy((destination->email), source_ptr + ID_SIZE + USERNAME_SIZE, EMAIL_SIZE);
}

Pager* pager_open(const char* filename) {
    Pager* pager = new Pager;

    pager->file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!pager->file_stream.is_open()) {
        pager->file_stream.clear();
        pager->file_stream.open(filename, std::ios::out | std::ios::binary);
        pager->file_stream.close();
        pager->file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    pager->file_stream.seekg(0, std::ios::end);
    pager->file_length = pager->file_stream.tellg();
    pager->file_stream.seekg(0, std::ios::beg);

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = nullptr;
    }

    return pager;
}

Table* db_open(const char* filename) {
    Pager* pager_ptr = pager_open(filename);

    Table* table = new Table;
    table->pager = pager_ptr;
    table->root_page_num = 0;

    if (pager_ptr->file_length == 0) {
        void* root_node = get_page(pager_ptr, 0);
        initialize_leaf_node(root_node);
    }

    return table;
}

void* get_page(Pager* pager, uint32_t page_num) {
    if (pager->pages[page_num] == nullptr) {
        void* page = new char[PAGE_SIZE];

        if (pager->file_length > (page_num * PAGE_SIZE)) {
            pager->file_stream.seekg(page_num * PAGE_SIZE, std::ios::beg);
            pager->file_stream.read(static_cast<char*>(page), PAGE_SIZE);
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void db_close(Table* table) {
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        if (table->pager->pages[i] != nullptr) {
            table->pager->file_stream.seekp(i * PAGE_SIZE, std::ios::beg);
            table->pager->file_stream.write(static_cast<char*>(table->pager->pages[i]), PAGE_SIZE);
            delete[] static_cast<char*>(table->pager->pages[i]);
        }
    }

    table->pager->file_stream.close();
    delete table->pager;
    delete table;
}

Cursor* table_start(Table* table) {
    Cursor* cursor = new Cursor;
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;

    void* root_node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    cursor->end_of_table = (num_cells == 0);

    return cursor;
}

void* cursor_value(Cursor* cursor) {
    uint32_t page_num = cursor->page_num;
    void* page = get_page(cursor->table->pager, page_num);
    return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor* cursor) {
    uint32_t page_num = cursor->page_num;
    void* node = get_page(cursor->table->pager, page_num);
    
    cursor->cell_num += 1;
    if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
        cursor->end_of_table = true;
    }
}

Cursor* table_find(Table* table, uint32_t key) {
    uint32_t root_page_num = table->root_page_num;
    void* root_node = get_page(table->pager, root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);

    Cursor* cursor = new Cursor;
    cursor->table = table;
    cursor->page_num = root_page_num;

    uint32_t min_index = 0;
    uint32_t max_index = num_cells;

    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(root_node, index);

        if (key == key_at_index) {
            cursor->cell_num = index;
            cursor->end_of_table = false;
            return cursor;
        }
        if (key < key_at_index) {
            max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    cursor->cell_num = min_index;
    cursor->end_of_table = (min_index >= num_cells);
    return cursor;
}