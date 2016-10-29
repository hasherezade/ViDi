#pragma once

//#include "udis/UDisasm.h"
#include "UDisasm.h"

namespace minidis {

class ExeDisasm : public UDisasm {
public:
    ExeDisasm(Executable *exe, offset_t startOffset) : UDisasm(exe, startOffset) {}

    virtual bool init(const offset_t startOffset, const bufsize_t disasmSize)
    {
        return UDisasm::init(startOffset, disasmSize, m_Exe->getBitMode());
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const = 0;
    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const = 0;

    virtual bool isFollowable(const size_t index) const
    {
        return UDisasm::isFollowable(index);
    }

    bool isBranching(size_t index)
    {
        mnem_type mType = this->getMnemTypeAtIndex(index);
        return UDisasm::isBranching(mType);
    }

    bool isBranching(offset_t offset, Executable::addr_type aType)
    {
        size_t index = this->offsetToIndex(offset, aType);
        if (index == INVALID_INDEX) return false;

        mnem_type mType = this->getMnemTypeAtIndex(index);
        return UDisasm::isBranching(mType);
    }

    bool isFollowable(size_t index)
    {
        return UDisasm::isFollowable(index);
    }

    bool isFollowable(offset_t offset, Executable::addr_type aType)
    {
        size_t index = this->offsetToIndex(offset, aType);
        if (index == INVALID_INDEX) return false;

        return UDisasm::isFollowable(index);
    }
};

}; /* namespace minidis */
