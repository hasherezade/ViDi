#pragma once

#include <vector>
#include <bearparser/core.h>

#define INVALID_INDEX (-1)

namespace minidis {

class ForkPoint
{
public:
    ForkPoint()
        : forkOffset(INVALID_ADDR), yesOffset(INVALID_ADDR), noOffset(INVALID_ADDR){ ; }

    ForkPoint(offset_t v_forkOffset, offset_t v_yesOffset, offset_t v_noOffset)
        : forkOffset(v_forkOffset), yesOffset(v_yesOffset), noOffset(v_noOffset)
    {
        //printf("Fork at %x -> (%x | %x)\n", forkOffset, yesOffset, noOffset);
    }

    offset_t forkOffset;
    offset_t yesOffset;
    offset_t noOffset;
};

class CodeBlock {

public:
    CodeBlock() : start(INVALID_ADDR), size(0) {}
    CodeBlock(offset_t v_start) : start(v_start), size(0), m_isInvalid(false) {}

    bool append(offset_t offset)
    {
        if (offset == INVALID_ADDR) return false;

        this->offsets << offset;
        this->size = offset - start;
        return true;
    }

    bool contains(offset_t offset) { return offsets.contains(offset); }
    
    offset_t getEndOffset()
    {
        if (offsets.size() == 0) return INVALID_INDEX;
        return offsets.back();
    }
    
    void  markInvalid() { m_isInvalid = true; }
    bool isInvalid() { return m_isInvalid; }

    int getIndexOf(offset_t offset)
    {
        const int index = offsets.indexOf(offset);
        if (index == -1) return INVALID_INDEX;
        return index;
    }

    Executable::addr_type getAddrType() { return Executable::RAW; } //block keeps RAW offsets

    offset_t start;
    offset_t size;
    
    QList<offset_t> offsets;
protected:
    bool m_isInvalid;
};

}; /* namespace minidis */
