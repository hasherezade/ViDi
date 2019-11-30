#include "pe/PeDisasm.h"

using namespace minidis;

PeDisasm::PeDisasm(PEFile *pe)
    : ExeDisasm(pe, 0), m_PE(pe)
{
    if (pe == NULL) throw CustomException("PE not initialized!");
}

bool PeDisasm::fillTable(const DisasmSettings& settings)
{
    return ExeDisasm::fillTable(settings);
}
