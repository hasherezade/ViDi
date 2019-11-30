#pragma once

#include <bearparser.h>

#include "DisasmChunkBuf.h"

#include "AddrConverter.h"
#include "FuncNameManager.h"

// maximal number of chunks that the disassembler is allowed to generate at the time

#define MAX_DISASM_EL 1000

namespace minidis {
//------------------------------------------------

    class DisasmSettings {
    public:
        DisasmSettings()
        {
           m_maxDisasmElements = MAX_DISASM_EL;
           m_stopAtBlockEnd = true;
           m_stopAtFuncEnd = true;
        }
        
        size_t getMaxDisasmElements() const
        {
            return m_maxDisasmElements;
        }
        
        bool isStopAtBlockEnd() const
        {
            return m_stopAtBlockEnd;
        }
        
        bool isStopAtFuncEnd() const
        {
            return m_stopAtFuncEnd;
        }
        
    protected:
        size_t m_maxDisasmElements;
        bool m_stopAtBlockEnd;
        bool m_stopAtFuncEnd;
        
        friend class Tracer;
        friend class DisasmBase;
    };
    

/* abstract class, base of all the Disasms*/
class DisasmBase : public AddrConverter
{
public:
/* static*/
    const static size_t MAX_ARG_NUM;

    static bool isBlockRet(const mnem_type &mType);
    static bool isBlockEnd(const mnem_type &mType);
    static bool isJump(const mnem_type &mType);

    static bool isBranching(const mnem_type &mType);
    static bool isConditionalBranching(const mnem_type &mType);
    static bool isUnonditionalBranching(const mnem_type &mType);

/* non-static */
    DisasmBase(Executable* exe, offset_t startOffset) :
        m_Exe(exe), m_startOffset(startOffset),
        is_init(false),
        m_addrType(Executable::RAW)//,m_nameManager(this)
    {
    }

    virtual ~DisasmBase()
    {
        is_init = false;
    }

    DisasmChunk* getChunkAtIndex(size_t index) const
    {
        return m_disasmBuf.at(index);
    }
    
    virtual bool isBranching(offset_t offset, Executable::addr_type aType) = 0;
    
    /**
     * @brief fills table of disassembly chunks
     * @param settings set of conditions at which te filling of the table should stop
     * @return true if successful
     */
    virtual bool fillTable(const DisasmSettings &settings);

    virtual void clearTable()
    {
        m_disasmBuf.clear();
    }

    size_t offsetToIndex(const offset_t inOffset, Executable::addr_type inType) const
    {
        offset_t offset = this->convertAddr(inOffset, inType, m_addrType);
        return m_disasmBuf.offsetToIndex(offset);
    }

    //virtual cond_buf Disasm::getCond(offset_t offset, Executable::addr_type inType) const = 0; //TODO
    virtual mnem_type getMnemTypeAtIndex(const size_t index) const
    {
        DisasmChunk* chunk = this->m_disasmBuf.at(index);
        if (!chunk) {
            return MT_NONE;
        }
        return chunk->getMnemType();
    }

    virtual mnem_type getMnemTypeAtOffset(offset_t offset, Executable::addr_type inType) const
    {
        size_t index = offsetToIndex(offset, inType);
        if (index == INVALID_INDEX) return MT_NONE;

        return getMnemTypeAtIndex(index);
    }

    virtual offset_t convertAddr(offset_t off, Executable::addr_type inType, Executable::addr_type outType) const
    {
        return m_Exe->convertAddr(off, inType, outType);
    }

    virtual Executable::addr_type detectAddrType(offset_t off, Executable::addr_type hintType) const
    {
        return m_Exe->detectAddrType(off, hintType);
    }

    virtual bool isFollowable(const size_t y) const;

    //wrapper:
    virtual bool isFollowable(offset_t offset, Executable::addr_type aType) const
    {
        offset = this->convertAddr(offset, aType, Executable::RAW);
        size_t index = this->m_disasmBuf.offsetToIndex(offset);
        return isFollowable(index);
    }

    virtual offset_t getTargetOffset(const size_t index, Executable::addr_type aType) const;

    virtual offset_t getOffset(const size_t index, const Executable::addr_type aType = Executable::RAW) const
    {
        const DisasmChunk *chunk = this->getChunkAtIndex(index);
        if (!chunk) return INVALID_ADDR;

        const offset_t raw = m_disasmBuf.indexToOffset(index);
        return this->convertAddr(raw, Executable::RAW, aType);
    }

    virtual offset_t getNextOffset(const size_t index, const Executable::addr_type aType = Executable::RAW) const
    {
        if (!m_Exe) return INVALID_ADDR;
        
        DisasmChunk *chunk = this->getChunkAtIndex(index);
        if (!chunk) return INVALID_ADDR;

        offset_t raw = getOffset(index, Executable::RAW);
        if (raw == INVALID_ADDR) return INVALID_ADDR;
        
        raw += chunk->getChunkSize();
        if (raw >= m_Exe->getContentSize()) return INVALID_ADDR;
        
        return this->convertAddr(raw, Executable::RAW, aType);
    }

    virtual bool isPushRet(int pushIndex) const; // equivalent of CALL
    virtual bool isCallToRet(const size_t callIndex) const; // equivalent of NOP

    //virtual QString translateBranching(const size_t index, FuncNameManager *nameManager = NULL) const = 0;
    bufsize_t getChunkSize(const size_t index) const { return this->m_disasmBuf.getChunkSize(index); } 

    bool hasOffset(offset_t offset) { return (m_disasmBuf.offsetToIndex(offset) != INVALID_INDEX); }

    virtual bool isImportCall(size_t index) { return false; }
    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const { return false; }
    virtual bool isImportCall(offset_t offset, Executable::addr_type inType);
    virtual bool isInternalCall(offset_t offset, Executable::addr_type inType);
    virtual bool isInterruptX(size_t index);

/* wrappers */
    bool isBlockRet(const size_t index) const { return isBlockRet(getMnemTypeAtIndex(index)); }
    bool isBlockEnd(const size_t index) const { return isBlockEnd(getMnemTypeAtIndex(index)); }
    bool isJump(const size_t index) const { return isJump(getMnemTypeAtIndex(index)); }

    virtual bool isBranching(size_t index) = 0;
    bool isConditionalBranching(size_t index) const { return isConditionalBranching(getMnemTypeAtIndex(index)); }
    bool isUnonditionalBranching(size_t index) const { return isUnonditionalBranching(getMnemTypeAtIndex(index)); }

    target_state getTargetState(const size_t index) const; //TODO: refactor it
//----
    DisasmChunkBuf m_disasmBuf;

protected:
    virtual DisasmChunk* makeChunk(offset_t startRVA) = 0;
    
    // distance between current RVA and Target RVA
    int32_t getTargetDelta(const size_t index) const;

    static uint64_t trimToBitMode(int64_t value, Executable::exe_bits bits);
    //---
    virtual size_t disasmNext() = 0;
    bool isOpToRet(const int index, const mnem_type op) const;
    virtual offset_t getImmediate(offset_t val, Executable::addr_type outType, Executable::addr_type hintType) const;

    bool is_init;
    Executable::exe_bits m_bitMode;
    Executable::addr_type m_addrType;
    offset_t m_startOffset;
    Executable* m_Exe;

}; /* class DisasmBase */

}; /* namespace minidis */
