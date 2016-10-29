#include "Context.h"

void minidis::resetCond(cond_buf &buf)
{
    buf.CF = FLAG_UNK;
    buf.PF = FLAG_UNK;
    buf.AF = FLAG_UNK;
    buf.ZF = FLAG_UNK;
    buf.SF = FLAG_UNK;
    buf.IF = FLAG_UNK;
    buf.DF = FLAG_UNK;
    buf.OF = FLAG_UNK;
    buf.cx = FLAG_UNK;
    buf.affectedCounter = 0;
}

