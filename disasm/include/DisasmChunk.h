#pragma once

#include <bearparser/core.h>

#include "AddrConverter.h"
#include "MnemType.h"
#include "Context.h"
#include "TargetValue.h"

namespace minidis {

class DisasmChunk : public AddrConverter {
public:
    const static size_t MAX_ARG_NUM;

    DisasmChunk(offset_t startOffset,Executable *parent)
        : m_Exe(parent), m_startOffset(startOffset), m_bitMode(parent->getBitMode())
    {
    }

    virtual ~DisasmChunk() {}

    /*inherited from AddrConverter */
    virtual offset_t convertAddr(offset_t off, Executable::addr_type inType, Executable::addr_type outType) const
    {
        if (!m_Exe) return off;
        return m_Exe->convertAddr(off, inType, outType);
    }

    virtual Executable::addr_type detectAddrType(offset_t off, Executable::addr_type hintType) const
    {
        if (!m_Exe) return hintType;
        return m_Exe->detectAddrType(off, hintType);
    }
//---
    virtual bool isValidAddr(offset_t addr, Executable::addr_type aType) const
    {
        if (!m_Exe) return false;
        return m_Exe->isValidAddr(addr, aType);

    }
    offset_t getImmediate(offset_t lval, Executable::addr_type outType, Executable::addr_type hintType) const;

    virtual bufsize_t getChunkSize() const = 0;
    virtual QString translateBranchingMnemonic() const = 0;

    virtual QString toString()
    {
        //lazy init
        if (m_disasmString.length() == 0) initStrings(); 
        return m_disasmString;
    }

    virtual QString toHexString()
    {
        //lazy init
        if (m_hexStr.length() == 0) initStrings(); 
        return m_hexStr;
    }

    virtual mnem_type getMnemType() const { return this->m_mnemType; }


    virtual bool isBranching() const;
    virtual bool isConditionalBranching() const;
    virtual bool isUnconditionalBranching() const;
    virtual bool isJump() const;

    bool isFuncEnd() const;
    bool isRet() const;

    offset_t getTargetRaw() const { return this->m_targetVal.getTargetRaw(); }
    offset_t getTargetAddr() const { return this->m_targetVal.getTargetAddr(); }
    Executable::addr_type getTargetAddrType() const { return m_targetVal.getAddrType(); }
    bool isTargetImm() const {return m_targetVal.isImm(); }

protected:
    QString printBytes(const uint8_t* buf, const size_t size) const
    {
        QString str;
        for (size_t i = 0; i < size; i++) {
            const uint8_t num = buf[i];
            str += QString::number(num, 16).leftJustified(2,'0');
        }
        return str;
    }
    //---
    /*
    virtual void init()
    {
        m_mnemType = fetchMnemType();
        m_targetVal = fetchTargetAddr();
    }
    */
    virtual void initStrings() = 0;

    virtual mnem_type fetchMnemType() const = 0;
    virtual size_t getMaxArg() = 0;
    //virtual bool fetchTargetAddr(const size_t argNum, TargetValue &targetVal) const = 0;
    //virtual TargetValue fetchTargetAddr() = 0;

    uint64_t trimToBitMode(int64_t value, Executable::exe_bits bits) const
    {
        uint64_t lval = value;
        const size_t max_bits = sizeof(lval) * 8;
        const size_t dif = max_bits - bits;
        lval = (lval << dif) >> dif;
        return lval;
    }

    int64_t signExtend(int64_t operand, size_t opSize) const
    {
        size_t opBits = opSize * 8;
        int64_t lval = operand;
        size_t dif = sizeof(lval) * 8 - opBits;
        lval = (operand << dif) >> dif;
        return lval;
    }

    bool setTargetVal(TargetValue &targetVal, const size_t argNum, Executable::addr_type aType, int64_t lval, bool isImm = false) const
    {
        if (lval == INVALID_ADDR) {
            targetVal.setValues(argNum, aType, INVALID_ADDR, INVALID_ADDR, isImm);
            return false;
        }

        offset_t raw = this->convertAddr(lval, aType, Executable::RAW);
        targetVal.setValues(argNum, aType, lval, raw, isImm);
        return true;
    }

    virtual offset_t getOffset() const = 0;
//---
    TargetValue m_targetVal;
    offset_t m_startOffset;

    QString m_disasmString, m_hexStr;
    mnem_type m_mnemType;

    Executable::exe_bits m_bitMode;
    Executable* m_Exe;

friend class DisasmChunkBuf;
};

//---
}; // namespace minidis
