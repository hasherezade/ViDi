#pragma once

#include <vector>
#include <bearparser.h>

#include "Tracer.h"
#include "DosDisasm.h"

namespace minidis {

class DosTracer : public Tracer
{
public:
    DosTracer(DOSExe *exe)
        : Tracer(exe) , m_dos(exe){ }
    
    void traceEntrySection();
    bool traceFunction(offset_t offset, Executable::addr_type aType, QString name, bool stopAtBlockEnd = true);

    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const
    {
        return false;
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const
    {
        return "?";
    }

protected:
    virtual DosDisasm* makeDisasm(Executable* exe, offset_t startRaw);

    DOSExe *m_dos;
};

}; /* namespace minidis */
