#pragma once

#include <bearparser/core.h>

BYTE* find_pattern(BYTE *buffer, size_t buf_size, BYTE* pattern_buf, size_t pattern_size, size_t max_iter = 0)
{
    for (size_t i = 0; (i + pattern_size) < buf_size; i++) {
        if (max_iter != 0 && i > max_iter) break;
        if (memcmp(buffer + i, pattern_buf, pattern_size) == 0) {
            return (buffer + i);
        }
    }
    return NULL;
}
