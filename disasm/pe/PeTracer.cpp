#include "pe/PeTracer.h"
#include "../Util.h"

using namespace minidis;

#define MAX_CHUNKS 10000

//----
class Pattern {
public:
    Pattern()
        : buf(NULL), size(0)
    {
    }
    
    Pattern(BYTE *bufPtr, size_t bufSize)
    {
        this->buf = bufPtr;
        this->size = bufSize;
    }
    
    BYTE* buf;
    size_t size;
};

//----
//protected:
size_t PeTracer::findAllPrologs(QSet<offset_t> &prologOffsets)
{
    size_t initialSize = prologOffsets.size();
    
    const offset_t epRaw = m_PE->getEntryPoint(Executable::RAW);
    SectionHdrWrapper *hdr = m_PE->getSecHdrAtOffset(epRaw, Executable::RAW, false);
    if (!hdr) return 0;
    
    const offset_t secStart = hdr->getContentOffset(Executable::RAW, false);
    const bufsize_t secSize = hdr->getContentSize(Executable::RAW, false);
    
    BYTE *secPtr = m_PE->getContentAt(secStart, secSize);
    if (!secPtr) {
        return 0;
    }
    BYTE prolog32_pattern[] = {
        0x55, // PUSH EBP
        0x8b, 0xEC // MOV EBP, ESP
    };

    BYTE prolog32_2_pattern[] = {
        0x55, // PUSH EBP
        0x89, 0xE5 // MOV EBP, ESP
    };
    
    BYTE prolog64_pattern[] = {
        0x40, 0x53, // PUSH RBX
        0x48, 0x83, 0xEC // SUB RSP, ??
    };
    
    QVector<Pattern> patterns;
    patterns.push_back(Pattern(prolog32_pattern, sizeof(prolog32_pattern)));
    patterns.push_back(Pattern(prolog32_2_pattern, sizeof(prolog32_2_pattern)));
    patterns.push_back(Pattern(prolog64_pattern, sizeof(prolog64_pattern)));
    
    BYTE *nextPtr = secPtr;
    size_t nextSize = secSize;
    
    for (nextPtr = secPtr; nextPtr < (secPtr + secSize);) {
        if (!nextPtr) break;
        
        size_t nextSize = secSize - (nextPtr - secPtr);
        BYTE *patternPtr = NULL;
        size_t patternSize = 0;
        
        //search by stored patterns:
        for (int i = 0; i < patterns.size(); i++) {
            patternPtr = find_pattern(nextPtr, nextSize, patterns.at(i).buf, patterns.at(i).size);
            if (!patternPtr) {
                continue;
            }
            offset_t offset1 = m_PE->getOffset(patternPtr);
            if (offset1 == INVALID_ADDR) {
                continue;
            }
            //pattern found!
            patternSize =  patterns.at(i).size;
            prologOffsets.insert(offset1);
            nextPtr = patternPtr + patternSize;
            break;
        }
        //none of the pattern was found, break:
        if (!patternPtr || !patternSize) break;
        
        //otherwise search for the next occurence...
    }
    return prologOffsets.size() - initialSize;
}

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
