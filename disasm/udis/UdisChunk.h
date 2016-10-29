#pragma once

#include <bearparser.h>
#include <udis86.h>

#include "../DisasmChunk.h"

namespace minidis {

class UdisChunk : public DisasmChunk 
{
public:
    UdisChunk(ud_t udObj, offset_t startOffset,Executable *parent) 
        : ud_obj(udObj), DisasmChunk(startOffset, parent)
    {
        m_mnemType = fetchMnemType();
        m_targetVal = fetchTargetAddr();
    }

    virtual bufsize_t getChunkSize() const { return ud_insn_len(&ud_obj);  }

    virtual QString translateBranchingMnemonic() const;
    virtual bool isAddrOperand() const;

protected:
    virtual void initStrings()
    {
        m_disasmString = QString(ud_obj.asm_buf_int);
        m_hexStr = QString::fromAscii(ud_insn_hex(&ud_obj)).toUpper();
    }

    virtual mnem_type fetchMnemType() const;
    virtual size_t getMaxArg() { return MAX_ARG_NUM; }

    virtual offset_t getOffset() const { return ud_insn_off(&ud_obj); }

private:
    bool fetchTargetAddr(const size_t argNum, TargetValue &targetVal) const;
    TargetValue fetchTargetAddr();
    static int64_t getSignedLVal(const ud_t &inpObj, size_t argNum, bool &isOk);
    ud_t ud_obj;

friend class UDisasm;
};

}; // namespace minidis
