#pragma once

#include <bearparser.h>
#include "../DisasmBase.h"

#include "UdisChunk.h"

namespace minidis {
//------------------------------------------------

/* abstract class, base of all the Disasms*/
class UDisasm : public DisasmBase
{
public:

/* non-static */
    UDisasm(Executable* exe, offset_t startOffset)
        : DisasmBase(exe, startOffset)
    {
        //init();
    }

    bool init(offset_t rva, bufsize_t disasmSize, Executable::exe_bits bitMode);

protected:

    virtual DisasmChunk* makeChunk(offset_t startRVA);
    
    static uint64_t trimToBitMode(int64_t value, Executable::exe_bits bits);
    //---
    virtual size_t disasmNext();
    int64_t getSignedLVal(const ud_t &obj, size_t operandNum, bool &isOk) const; /* TODO: test it!!! */

private:

    BYTE *m_buf;
    bufsize_t m_bufSize;
    bufsize_t m_disasmSize;

    offset_t m_iptr; //instruction pointer
    ud_t ud_obj;

}; /* class DisasmBase */

}; /* namespace minidis */
