#pragma once

#include <bearparser/pe.h>

namespace minidis {

class PeDataFetcher
{
public:
static bool isImportedFunction(PEFile *m_PE, offset_t offset, Executable::addr_type aType)
{
    if (!m_PE) return false;
    if (offset == INVALID_ADDR) return false;
    Executable::addr_type detectedType = m_PE->detectAddrType(offset, aType);
    offset_t rva = m_PE->convertAddr(offset, detectedType, Executable::RVA);

    ImportDirWrapper *imports = m_PE->getImportsDir();
    if (imports) {
        if (imports->hasThunk(rva)) return true;
    }
    DelayImpDirWrapper* delayedImps = m_PE->getDelayedImportsDir();
    if (delayedImps) {
        
        
        if (delayedImps->hasThunk(rva)) {
            //printf("IN Delayed imports!\n");
            return true;
        }
        offset_t va = m_PE->convertAddr(offset, detectedType, Executable::VA);
        if (delayedImps->hasThunk(va)) {
            //printf("IN Delayed imports!\n");
            return true;
        }
    }
    return false;
}

static QString getImportName(PEFile *m_PE, offset_t offset, Executable::addr_type aType)
{
    if (!m_PE) return "";
    if (offset == INVALID_ADDR) return "";
    Executable::addr_type detectedType = m_PE->detectAddrType(offset, aType);
    offset_t rva = m_PE->convertAddr(offset, detectedType, Executable::RVA);

    ImportDirWrapper *imports = m_PE->getImportsDir();
    if (imports) {
        if (imports->hasThunk(rva)) {
            return imports->thunkToLibName(rva) + "." + imports->thunkToFuncName(rva);
        }
    }
    DelayImpDirWrapper* delayedImps = m_PE->getDelayedImportsDir();
    if (delayedImps) {
        if (delayedImps->hasThunk(rva)) {
            return delayedImps->thunkToLibName(rva) + ":" + delayedImps->thunkToFuncName(rva);
        }
        offset_t va = m_PE->convertAddr(offset, detectedType, Executable::VA);
        if (delayedImps->hasThunk(va)) {
            return delayedImps->thunkToLibName(va) + ":" + delayedImps->thunkToFuncName(va);
        }
    }
    return "?";
}

};
}; /* namespace minidis */
