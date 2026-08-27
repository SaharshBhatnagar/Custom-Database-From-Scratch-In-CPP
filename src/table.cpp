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

    void* page = get_page(table->pager, page_num);

    uint32_t row_offset = row_num % ROWS_PER_PAGE;

    uint32_t byte_offset = row_offset * ROW_SIZE;

    return static_cast<char*>(page) + byte_offset;

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

    for (uint32_t i = 0; i < 100; i++) {
        pager->pages[i] = nullptr;
    }

    return pager;
}

Table* db_open(const char* filename) {
    Pager* pager_ptr = pager_open(filename);

    Table* table = new Table;
    table->pager = pager_ptr;

    table->num_rows = pager_ptr->file_length / ROW_SIZE;

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
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (table->pager->pages[i] != nullptr) {
            
            table->pager->file_stream.seekp(i * PAGE_SIZE, std::ios::beg);

            table->pager->file_stream.write(static_cast<char*>(table->pager->pages[i]), PAGE_SIZE);

            delete[] static_cast<char*>(table->pager->pages[i]);

        }
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;

        if (num_additional_rows > 0) {
            if (table->pager->pages[num_full_pages] != nullptr) {

                table->pager->file_stream.seekp(num_full_pages * PAGE_SIZE, std::ios::beg);

                table->pager->file_stream.write(static_cast<char*>(table->pager->pages[num_full_pages]), num_additional_rows * ROW_SIZE);

                delete[] static_cast<char*>(table->pager->pages[num_full_pages]);
        }
    }

    table->pager->file_stream.close();
    delete table->pager;
    delete table;
}