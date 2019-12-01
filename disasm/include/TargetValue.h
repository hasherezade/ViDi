#pragma once

#include <bearparser/core.h>

namespace minidis {

enum target_state {
    TS_NOT_ADDR,
    TS_VIRTUAL_ONLY, //cannot be converted to raw
    TS_VALID,
    COUNT_TS
};

class TargetValue
{
public:
    TargetValue() : m_targetAddrType(Executable::NOT_ADDR),
        m_targetOpNum(0), m_isImm(false),
        m_targetAddr(INVALID_ADDR),  m_targetRaw(INVALID_ADDR) {}

    virtual ~TargetValue() {}

    Executable::addr_type getAddrType() const {return m_targetAddrType; }
    offset_t getTargetAddr() const { return m_targetAddr; }
    offset_t getTargetRaw() const { return m_targetRaw; } //converted

    size_t getOpNum() const { return m_targetOpNum; }
    bool isImm() const { return m_isImm; }
    target_state getState()
    {
        if (m_targetAddr == INVALID_ADDR) return TS_NOT_ADDR;
        if (m_targetRaw == INVALID_ADDR) return TS_VIRTUAL_ONLY;
        return TS_VALID;
    }

    void setValues(size_t targetOpNum, Executable::addr_type targetAddrType, offset_t targetAddr, offset_t targetRaw, bool isImm = false)
    {
        m_targetAddrType = targetAddrType; //should be VA or RVA
        m_targetAddr = targetAddr;
        m_targetRaw = targetRaw;
        m_targetOpNum = targetOpNum;
        m_isImm = isImm;
    }

// values:
    Executable::addr_type m_targetAddrType; // original type
    offset_t m_targetAddr; //original addr
    offset_t m_targetRaw;
    size_t m_targetOpNum;
    bool m_isImm;
};
//---
}; // namespace minidis
