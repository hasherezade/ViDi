#pragma once

#include <bearparser/core.h>

namespace minidis {

enum flag_val { FLAG_UNK = 0, FLAG_UNSET = (-1), FLAG_SET = 1, FLAG_REL, FLAG_RELNEG };

struct cond_buf {
    flag_val CF, PF, AF, ZF, SF, IF, DF, OF;
    int8_t cx;
    int8_t affectedCounter;
};

void resetCond(cond_buf &buf);

}; /* namespace minidis */
