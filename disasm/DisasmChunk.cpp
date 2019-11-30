#include "DisasmChunk.h"

using namespace minidis;

const size_t DisasmChunk::MAX_ARG_NUM = 2;

uint64_t trimToBitMode(int64_t value, Executable::exe_bits bits)
{
    uint64_t lval = value;
    size_t max_bits = sizeof(lval) * 8;
    size_t dif = max_bits - bits;
    lval = (lval << dif) >> dif;
    return lval;
}

offset_t DisasmChunk::getImmediate(offset_t lval, Executable::addr_type outType, Executable::addr_type hintType) const
{
    offset_t val = trimToBitMode(lval, m_bitMode);
    Executable::addr_type detectedType = this->detectAddrType(val, hintType);
    if (detectedType != hintType) return INVALID_ADDR;

    return convertAddr(val, detectedType, outType);
}

bool DisasmChunk::isFuncEnd() const
{
    mnem_type mnem = getMnemType();
    switch (mnem) {
        case MT_RET :
        case MT_INT3:
        case MT_INVALID:
            return true;
    }
    return false;
}

bool DisasmChunk::isRet() const
{
    mnem_type mnem = getMnemType();
    if (mnem == MT_RET) {
        return true;
    }
    return false;
}

/*
TargetValue DisasmChunk::fetchTargetAddr()
{
    TargetValue targetVal;
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);

    const size_t cnt = getMaxArg();
    if (cnt == 0) return targetVal;

    for (size_t i = 0; i < cnt; i++) {

        bool isOk = fetchTargetAddr(i, targetVal);
        if (!isOk || targetVal.getAddrType() == Executable::NOT_ADDR) continue;
        // OK:
        return targetVal;
    }
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);
    return targetVal;
}
*/
bool DisasmChunk::isBranching() const
{
    return isConditionalBranching() || isUnconditionalBranching();
}

bool DisasmChunk::isConditionalBranching() const
{
    switch (m_mnemType) {
        case MT_COND_JUMP:
        case MT_LOOP:
            return true;
    }
    return false;
}

bool DisasmChunk::isUnconditionalBranching() const
{
    switch (m_mnemType) {
        case MT_JUMP:
        case MT_CALL:
            return true;
    }
    return false;
}

bool DisasmChunk::isJump() const
{
    switch (m_mnemType) {
        case MT_JUMP:
        case MT_COND_JUMP:
            return true;
    }
    return false;
}
