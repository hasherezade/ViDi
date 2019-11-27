#pragma once

#include "../ExeDisasm.h"

#define PREVIEW_SIZE 0x200

namespace minidis {

class DosDisasm : public ExeDisasm
{

public:
    DosDisasm(DOSExe *dos);

    bool fillTable(bool stopAtBlockEnd, size_t maxElements);

    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const
    {
        return false;
    }

    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const
    {
        return "?";
    }

    virtual bool isImportCall(size_t index)
    {
        return false;
    }

protected:
    DOSExe *m_dos;

}; /* class DosDisasm */

}; /* namespace minidis */

