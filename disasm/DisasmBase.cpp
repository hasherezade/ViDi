#include "DisasmBase.h"

using namespace minidis;

const size_t DisasmBase::MAX_ARG_NUM = 2;

bool DisasmBase::isBlockRet(const mnem_type &mType)
{
    switch (mType) {
        case MT_RET:
        case MT_INT3:
        case MT_INVALID:
            return true;
    }
    return false;
}

bool DisasmBase::isJump(const mnem_type &mType)
{
    switch (mType) {
        case MT_JUMP:
        case MT_COND_JUMP:
            return true;
    }
    return false;
}

bool DisasmBase::isConditionalBranching(const mnem_type &mType)
{
    switch (mType) {
        case MT_COND_JUMP:
        case MT_LOOP:
            return true;
    }
    return false;
}

bool DisasmBase::isUnonditionalBranching(const mnem_type &mType)
{
    switch (mType) {
        case MT_JUMP:
        case MT_CALL:
            return true;
    }
    return false;
}

bool DisasmBase::isBranching(const mnem_type &mType)
{
    if (isJump(mType)) {
        return true;
    }
    switch (mType) {
        case MT_CALL:
        case MT_LOOP:
            return true;
    }
    return false;
}

bool DisasmBase::isBlockEnd(const mnem_type &mType)
{
    if (isJump(mType)) {
        return true;
    }
    if (isBlockRet(mType)) {
        return true;
    }
    return false;
}

bool DisasmBase::isImportCall(offset_t offset, Executable::addr_type inType)
{
    offset = this->convertAddr(offset, inType, Executable::RAW);
    size_t index = this->m_disasmBuf.offsetToIndex(offset);
    if (index == INVALID_INDEX) return false;

    return isImportCall(index);
}

bool DisasmBase::isInternalCall(offset_t offset, Executable::addr_type inType)
{
    size_t index = offsetToIndex(offset, inType);

    if (index == INVALID_INDEX) return false;
    if (!this->isFollowable(index) ) return false;

    minidis::mnem_type mType = this->getMnemTypeAtIndex(index);

    if (mType != MT_CALL) return false;
    if (isImportCall(offset, inType)) return false;
    return true;
}

bool DisasmBase::isOpToRet(const int index, const mnem_type op) const
{
    DisasmChunk *dChunk = this->getChunkAtIndex(index);
    if (!dChunk || dChunk->getMnemType() != op) return false;

    //is pointer to RET?
    offset_t raw = getTargetOffset(index, Executable::RAW);
    if (raw == INVALID_ADDR) {
        return false;
    }
    BYTE *cntnt = m_Exe->getContentAt(raw, Executable::RAW, sizeof(OP_RET));
    if (cntnt != NULL && (*cntnt) == OP_RET) {
        return true;
    }
    return false;
}

bool DisasmBase::isCallToRet(const size_t index) const
{
    return isOpToRet(index, MT_CALL);
}

bool DisasmBase::isPushRet(int index) const
{
    DisasmChunk *dChunk = this->getChunkAtIndex(index);
    if (!dChunk || dChunk->getMnemType() != MT_PUSH) return false;

    offset_t nRaw = this->getNextOffset(index);
    BYTE *cntnt = m_Exe->getContentAt(nRaw, Executable::RAW, sizeof(OP_RET));
    if (cntnt != NULL && (*cntnt) == OP_RET) {
        return true;
    }
    return false;
}

bool DisasmBase::isInterruptX(size_t index)
{
   return (getMnemTypeAtIndex(index) == MT_INTX);
}

offset_t DisasmBase::getImmediate(offset_t lval, Executable::addr_type outType, Executable::addr_type hintType) const
{
    offset_t val = DisasmBase::trimToBitMode(lval, m_bitMode);
    Executable::addr_type detectedType = this->detectAddrType(val, hintType);
    //strict type check:
    if (detectedType != hintType) {
        return INVALID_ADDR;
    }
    return convertAddr(val, detectedType, outType);
}

uint64_t DisasmBase::trimToBitMode(int64_t value, Executable::exe_bits bits)
{
    uint64_t lval = value;
    size_t max_bits = sizeof(lval) * 8;
    size_t dif = max_bits - bits;
    lval = (lval << dif) >> dif;
    return lval;
}

int32_t DisasmBase::getTargetDelta(const size_t index) const
{
    offset_t rva = getTargetOffset(index, Executable::RVA);
    if (rva == INVALID_ADDR) return 0;

    offset_t currRVA = getOffset(index, Executable::RVA);
    int32_t delta = static_cast<int32_t>(rva - currRVA);
    return delta;
}

bool DisasmBase::isFollowable(const size_t index) const
{
    DisasmChunk *uChunk =  m_disasmBuf.at(index);
    if (!uChunk) return false;

    if (uChunk->isBranching() == false && isPushRet(index) == false) {
        return false;
    }
    offset_t targetRVA = getTargetOffset(index, Executable::RVA);
    if (targetRVA == INVALID_ADDR) {
        return false;
    }
    if (this->isImportedFunction(targetRVA, Executable::RVA)) {
        return false;
    }
    return true;
}

target_state DisasmBase::getTargetState(const size_t index) const
{
    //TODO: set this value during parsing target!
    const offset_t targetRaw = getTargetOffset(index, Executable::RAW);
    const offset_t targetRVA = getTargetOffset(index, Executable::RVA);

    if (targetRaw == INVALID_ADDR && targetRVA == INVALID_ADDR) {
        //target does not exist
        return TS_NOT_ADDR;
    }
    if (targetRaw != INVALID_ADDR) {
        return TS_VALID;
    }
    return TS_VIRTUAL_ONLY;
}

offset_t DisasmBase::getTargetOffset(const size_t index, Executable::addr_type aType) const
{
    if (index >= this->m_disasmBuf.size()) return INVALID_ADDR;
    DisasmChunk *uChunk = this->m_disasmBuf.at(index);
    if (!uChunk) return INVALID_ADDR;

    if (aType == Executable::RAW) {
        return uChunk->getTargetRaw();
    }
    return convertAddr(uChunk->getTargetAddr(), uChunk->getTargetAddrType(), aType);
}

bool DisasmBase::fillTable(const DisasmSettings &settings)
{
    if (this->is_init == false) {
        printf("Not initialized!\n");
        return false;
    }
    
    const size_t maxElements = settings.getMaxDisasmElements();
    bool stopAtBlockEnd = settings.isStopAtBlockEnd();
    bool stopAtFuncEnd = settings.isStopAtFuncEnd();
    
    size_t index = 0;
    offset_t startRVA = this->convertAddr(m_startOffset, Executable::RAW, Executable::RVA);
    for (index = 0; disasmNext() > 0; index ++) {
        
        DisasmChunk *uChunk = makeChunk(startRVA);
        if (!uChunk) break;

        m_disasmBuf.append(uChunk);
        //if (index < 2) printf("+%s\n", uChunk->toString().toStdString().c_str());
        if (m_disasmBuf.size() == maxElements) {
            stopAtBlockEnd = true;
        }
        if (m_disasmBuf.indexToOffset(index) == INVALID_ADDR) {
            break;
        }
        if (stopAtFuncEnd) {
            if (uChunk->isFuncEnd()) {
                //it is a block end, but not a branching
                break;
            }
        }
        if (stopAtBlockEnd) {
            if (uChunk->isFuncEnd() || uChunk->isBranching()) {
                //printf("+%s\n", uChunk->toString().toStdString().c_str());
                break;
            }
        }
    }
    return true;
}