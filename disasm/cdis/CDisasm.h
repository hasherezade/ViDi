#pragma once

#include <bearparser.h>
#include "DisasmBase.h"

#include "CapstoneChunk.h"

namespace minidis {
//------------------------------------------------
/* abstract class, base of all the Disasms*/
class CDisasm : public DisasmBase
{
public:

    CDisasm(Executable* exe, offset_t startOffset)
        : DisasmBase(exe, startOffset), m_insn(NULL) 
    { }
    
    ~CDisasm()
    {
        if (m_insn){
            cs_free(m_insn, 1);
            cs_close(&handle); //TODO: check if ever opened
        }
    }

    bool init(const offset_t startOffset, const bufsize_t disasmSize, Executable::exe_bits bitMode);
    
    //virtual bool fillTable(bool stopAtBlockEnd, size_t maxElements);
    virtual bool fillTable(const DisasmSettings &settings);

protected:
    virtual size_t disasmNext();

    bool init_capstone(Executable::exe_bits bitMode);
    size_t capstone_next(const BYTE *code, size_t size, offset_t startRaw);

    csh handle;
    cs_insn* m_insn;

    BYTE *m_csBuf;
    bufsize_t m_csBufSize;
    offset_t m_csOffset;

    BYTE *m_buf;
    bufsize_t m_bufSize;
    bufsize_t m_disasmSize;

}; /* class DisasmBase */

}; /* namespace minidis */
