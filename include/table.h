#pragma once
#include "row.h"
#include <cstdint>
#include <fstream>

const uint32_t PAGE_SIZE = 4096;

const uint32_t TABLE_MAX_PAGES = 100;

const uint32_t ID_SIZE = sizeof(uint32_t);

const uint32_t USERNAME_SIZE = sizeof(char) * (COLUMN_USERNAME_SIZE + 1);

const uint32_t EMAIL_SIZE = sizeof(char) * (COLUMN_EMAIL_SIZE + 1);

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Pager {
    std::fstream file_stream;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
};

struct Table {
    uint32_t num_rows;
    Pager* pager;
};

Pager* pager_open(const char* filename);
Table* db_open(const char* filename);
void db_close(Table* table);

void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* row_slot(Table* table, uint32_t row_num);
void* get_page(Pager* pager, uint32_t page_num);