#include "dos/DosDisasm.h"

using namespace minidis;

DosDisasm::DosDisasm(DOSExe *dos)
    : ExeDisasm(dos, 0), m_dos(dos)
{
    if (dos == NULL) throw CustomException("DOS Exe not initialized!");
}

bool DosDisasm::fillTable(bool stopAtBlockEnd, size_t maxElements)
{
    return ExeDisasm::fillTable(stopAtBlockEnd, maxElements);
}

