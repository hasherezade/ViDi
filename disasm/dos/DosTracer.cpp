#include "dos/DosTracer.h"

using namespace minidis;

#define MAX_CHUNKS 10000

//----

void DosTracer::traceEntrySection()
{
    const offset_t codeStart = this->m_dos->convertAddr(0, Executable::RVA, Executable::RAW);
    makeDisasmAt(m_dos, codeStart, false, MAX_CHUNKS);
    traceArea(codeStart);
}

bool DosTracer::traceFunction(offset_t offset, Executable::addr_type aType, QString name, bool stopAtBlockEnd)
{
    const offset_t start = this->convertAddr(offset, aType, Executable::RAW);
    if (start == INVALID_ADDR) return false;

    if (!makeDisasmAt(m_Exe, start, stopAtBlockEnd)) {
        return false;
    }
    traceArea(start);
    return defineFunction(offset, aType, name);
}

DosDisasm* DosTracer::makeDisasm(Executable* exe, offset_t startRaw)
{
    DOSExe *dos = dynamic_cast<DOSExe*>(exe);
    if (!dos) return NULL;
    DosDisasm *disasm = new DosDisasm(dos);
    bufsize_t size = dos->getContentSize() - startRaw;
    if (!disasm->init(startRaw, size)) {
        delete disasm;
        disasm = NULL;
   }
   return disasm;
}
