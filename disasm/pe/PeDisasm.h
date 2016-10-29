#pragma once

#include "../ExeDisasm.h"
#include "PeDataFetcher.h"

#define PREVIEW_SIZE 0x200

namespace minidis {

class PeDisasm : public ExeDisasm
{

public:
    PeDisasm(PEFile *pe);

    bool init(offset_t startOffset)
    {
        bufsize_t disasmSize = m_PE->getContentSize() - startOffset;

        SectionHdrWrapper* hdr = this->m_PE->getSecHdrAtOffset(startOffset, Executable::RAW, false);
        if (hdr) {
            offset_t start =  startOffset - hdr->getContentOffset(Executable::RAW);
            bufsize_t secSize = hdr->getContentSize(Executable::RAW, false);
            disasmSize = secSize - start;
        }
        return ExeDisasm::init(startOffset, disasmSize);
    }

    bool fillTable(bool stopAtBlockEnd, size_t maxElements);

    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const
    {
        return PeDataFetcher::isImportedFunction(m_PE, offset, aType);
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const
    {
        return PeDataFetcher::getImportName(m_PE, offset, aType);
    }

    virtual bool isImportCall(size_t index)
    {
        offset_t target = getTargetOffset(index, Executable::RVA);
        if (target != INVALID_ADDR && this->isImportedFunction(target, Executable::RVA)) {
            return true;
        }
        return false;
    }

protected:
    offset_t getRawAt(const size_t index) const;

    PEFile *m_PE;

}; /* class PeDisasm */

}; /* namespace minidis */

