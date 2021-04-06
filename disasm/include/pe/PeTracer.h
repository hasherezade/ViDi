#pragma once

#include <vector>
#include <bearparser/pe.h>

#include "Tracer.h"
#include "PeDisasm.h"

namespace minidis {

class PeTracer : public Tracer, protected PeDataFetcher
{
public:
    PeTracer(PEFile *exe)
        : Tracer(exe) , m_PE(exe){ }

    void traceEntrySection();
    bool traceFunction(offset_t offset, Executable::addr_type aType, QString name);

    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const
    {
        return PeDataFetcher::isImportedFunction(m_PE, offset, aType);
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const
    {
        return PeDataFetcher::getImportName(m_PE, offset, aType);
    }

    protected:
    
    size_t findAllPrologs(QSet<offset_t> &prologOffsets);
    virtual PeDisasm* makeDisasm(Executable* exe, offset_t startRaw);

    PEFile *m_PE;
};

}; /* namespace minidis */
