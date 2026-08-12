#pragma once
#include <cstdint>

const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};