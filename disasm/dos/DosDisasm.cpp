#include "dos/DosDisasm.h"

using namespace minidis;

DosDisasm::DosDisasm(DOSExe *dos)
    : ExeDisasm(dos, 0), m_dos(dos)
{
    if (dos == NULL) throw CustomException("DOS Exe not initialized!");
}

bool DosDisasm::fillTable(const DisasmSettings& settings)
{
    return ExeDisasm::fillTable(settings);
}

