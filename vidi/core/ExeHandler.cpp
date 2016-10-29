#include "ExeHandler.h"

void ExeHandler::makeTracer()
{
   if (m_Tracer) {
       delete m_Tracer; 
       m_Tracer = NULL;
    }
    PEFile *pe = dynamic_cast<PEFile*>(m_Exe);
    if (pe) {
        m_Tracer = new PeTracer(pe);
        return;
    }
   DOSExe *dos = dynamic_cast<DOSExe*>(m_Exe);
    if (dos) {
        m_Tracer = new DosTracer(dos);
        return;
    }
}

bool ExeHandler::resolveOffset(offset_t offset, Executable::addr_type aType)
{
    if (!m_Tracer) return false;

    if (m_Tracer->resolveOffset(offset, aType)) {
        emit stateChanged();
        return true;
    }
    return false;
}

//tags management:

bool ExeHandler::setFunctionName(offset_t offset, Executable::addr_type inType, QString name)
{
    if ( m_Tracer->setSingleFunctionName(offset, inType, name)) {
        emit stateChanged();
        return true;
    }
    return false;
}

size_t ExeHandler::saveFunctionNames(const QString &fileName)
{
    return this->m_Tracer ? this->m_Tracer->saveFunctionNames(fileName): 0;
}
 
size_t ExeHandler::loadFunctionNames(const QString &fileName)
{
    if (!m_Tracer) return 0;

    size_t count = this->m_Tracer->loadFunctionNames(fileName);
    if (count > 0 ) {
        emit stateChanged();
    }
    return count;
}
