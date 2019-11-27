#pragma once

#include <bearparser.h>
#include <capstone/capstone.h>

#include "DisasmChunk.h"

namespace minidis {

class CapstoneChunk : public DisasmChunk 
{
public:
    CapstoneChunk(const cs_insn &insn, offset_t startOffset, Executable *parent) 
        : DisasmChunk(startOffset, parent),
        m_insn(insn)
    {
        m_maxArgs = insn.detail->x86.op_count;
        m_mnemType = fetchMnemType();
        m_targetVal = fetchTargetAddr(insn.detail);
    }

    virtual bufsize_t getChunkSize() const{ return this->m_insn.size; }
    virtual QString translateBranchingMnemonic() const;

protected:
    virtual void initStrings()
    {
        m_disasmString = QString(m_insn.mnemonic) + " " + QString(m_insn.op_str);
        m_hexStr = printBytes((uint8_t*) m_insn.bytes, m_insn.size);
    }

    virtual size_t getMaxArg() { return m_maxArgs; }

    mnem_type fetchMnemType() const
    {
        return CapstoneChunk::fetchMnemType(static_cast<x86_insn>(m_insn.id));
    }

    virtual offset_t getOffset() const { return this->m_insn.address; }
    bool isLongOp() const { return (this->m_insn.id == X86_INS_LCALL || this->m_insn.id == X86_INS_LJMP); }

private:
    virtual bool fetchTargetAddr(const size_t argNum, TargetValue &targetVal, cs_detail *detail) const;
    TargetValue fetchTargetAddr(cs_detail *detail);
    static mnem_type fetchMnemType(const x86_insn cMnem);
    
    const cs_insn m_insn;
    size_t m_maxArgs;

friend class CDisasm;
};

}; // namespace minidis
