#pragma once

#include "CDisasm.h"

namespace minidis {

class ExeDisasm : public CDisasm {
public:
    ExeDisasm(Executable *exe, offset_t startOffset) : CDisasm(exe, startOffset) {}

    virtual bool init(const offset_t startOffset, const bufsize_t disasmSize)
    {
        return CDisasm::init(startOffset, disasmSize, m_Exe->getBitMode());
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const = 0;
    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const = 0;
    
    virtual QString getMnemString(size_t index) const 
    {
        return CDisasm::getMnemStringAtIndex(index);
    }

    virtual bool isFollowable(const size_t index) const
    {
        return CDisasm::isFollowable(index);
    }

    bool isBranching(size_t index)
    {
        mnem_type mType = this->getMnemTypeAtIndex(index);
        return CDisasm::isBranching(mType);
    }

    bool isBranching(offset_t offset, Executable::addr_type aType)
    {
        size_t index = this->offsetToIndex(offset, aType);
        if (index == INVALID_INDEX) return false;

        mnem_type mType = this->getMnemTypeAtIndex(index);
        return CDisasm::isBranching(mType);
    }

    bool isFollowable(size_t index)
    {
        return CDisasm::isFollowable(index);
    }

    bool isFollowable(offset_t offset, Executable::addr_type aType)
    {
        size_t index = this->offsetToIndex(offset, aType);
        if (index == INVALID_INDEX) return false;

        return CDisasm::isFollowable(index);
    }
};

}; /* namespace minidis */
