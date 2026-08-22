#pragma once
#include "row.h"
#include <cstdint>

const uint32_t PAGE_SIZE = 4096;

const uint32_t TABLE_MAX_PAGES = 100;

const uint32_t ID_SIZE = sizeof(uint32_t);

const uint32_t USERNAME_SIZE = sizeof(char) * (COLUMN_USERNAME_SIZE + 1);

const uint32_t EMAIL_SIZE = sizeof(char) * (COLUMN_EMAIL_SIZE + 1);

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
};

void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* row_slot(Table* table, uint32_t row_num);