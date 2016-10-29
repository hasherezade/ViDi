#include "PeTracer.h"

using namespace minidis;

#define MAX_CHUNKS 10000

//----

void PeTracer::traceEntrySection()
{
    const offset_t epRaw = m_PE->getEntryPoint(Executable::RAW);
    SectionHdrWrapper *hdr = m_PE->getSecHdrAtOffset(epRaw, Executable::RAW, false);
    if (!hdr) return;

    const offset_t secStart = hdr->getContentOffset(Executable::RAW, false);
    makeDisasmAt(m_Exe, secStart, false, MAX_CHUNKS);
    traceArea(secStart);
}

bool PeTracer::traceFunction(offset_t offset, Executable::addr_type aType, QString name, bool stopAtBlockEnd)
{
    const offset_t start = this->convertAddr(offset, aType, Executable::RAW);
    if (start == INVALID_ADDR) return false;

    if (!makeDisasmAt(m_Exe, start, stopAtBlockEnd)) {
        return false;
    }
    traceArea(start);
    return defineFunction(offset, aType, name);
}

PeDisasm* PeTracer::makeDisasm(Executable* exe, offset_t startRaw)
{
    PEFile *pe = dynamic_cast<PEFile*>(exe);
    if (!pe) return NULL;
    PeDisasm *disasm = new PeDisasm(pe);
    if (!disasm->init(startRaw)) {
        delete disasm;
        disasm = NULL;
   }
   return disasm;
}
