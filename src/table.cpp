#include "table.h"
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

    std::memcpy((destination->email), source_ptr +ID_SIZE + USERNAME_SIZE, EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;

    if (table->pages[page_num] == nullptr) {
        table->pages[page_num] = new char [PAGE_SIZE];
    }

    uint32_t row_offset = row_num % ROWS_PER_PAGE;

    uint32_t byte_offset = row_offset * ROW_SIZE;

    return static_cast<char*>(table->pages[page_num]) + byte_offset;

}