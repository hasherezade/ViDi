#include "pe/PeTracer.h"
#include "../Util.h"

using namespace minidis;

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
    
    BYTE prolog64_pattern_0[] = {
        0x55, // PUSH RBP
        0x48, 0x8b, 0xEC // MOV RBP, RSP
    };
    
    BYTE prolog64_pattern_1[] = {
        0x40, 0x53, // PUSH RBX
        0x48, 0x83, 0xEC // SUB RSP, ??
    };
    
    BYTE prolog64_pattern_2[] = {
        0x40, 0x55, // PUSH ??
        0x48, 0x83, 0xEC // SUB RSP, ??
    };
    
    BYTE prolog64_pattern_3[] = {
        0x48, 0x83, 0xEC // SUB RSP, ??
    };
    //TODO: make a pattern covering all the cases
    QVector<Pattern> patterns;
    patterns.push_back(Pattern(prolog32_pattern, sizeof(prolog32_pattern)));
    patterns.push_back(Pattern(prolog32_2_pattern, sizeof(prolog32_2_pattern)));
    if (m_PE->isBit64()) {
        patterns.push_back(Pattern(prolog64_pattern_0, sizeof(prolog64_pattern_0)));
        patterns.push_back(Pattern(prolog64_pattern_1, sizeof(prolog64_pattern_1)));
        patterns.push_back(Pattern(prolog64_pattern_2, sizeof(prolog64_pattern_2)));
        patterns.push_back(Pattern(prolog64_pattern_3, sizeof(prolog64_pattern_3)));
    }
    BYTE *nextPtr = secPtr;
    size_t nextSize = secSize;
    
    BYTE hotpatch_buf[] = { 0x8b, 0xff }; //mov edi, edi
    Pattern hotpatch(hotpatch_buf, sizeof(hotpatch_buf));
    
    for (nextPtr = secPtr; nextPtr < (secPtr + secSize);) {
        if (!nextPtr) break;
        
        size_t nextSize = secSize - (nextPtr - secPtr);
        BYTE *patternPtr = NULL;
        size_t patternSize = 0;
        
        //search by stored patterns:
        for (int i = 0; i < patterns.size(); i++) {
            patternPtr = find_pattern(nextPtr, nextSize, patterns.at(i).buf, patterns.at(i).size);
            if (!patternPtr) {
                //try with another pattern
                continue;
            }
            //pattern found!
            patternSize =  patterns.at(i).size;
            //check for eventual hotpatch prolog:
            if ((patternPtr - secPtr) >= hotpatch.size) {
                BYTE *hotPatchPtr = patternPtr - hotpatch.size;
                if (memcmp(hotPatchPtr, hotpatch.buf, hotpatch.size) == 0) {
                    patternPtr = hotPatchPtr;
                    patternSize += hotpatch.size;
                }
            }
            //advance the search:
            nextPtr = patternPtr + patternSize;
            
            offset_t offset1 = m_PE->getOffset(patternPtr);
            if (offset1 != INVALID_ADDR) {
                prologOffsets.insert(offset1);
                break;
            }
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
    makeDisasmAt(m_Exe, secStart, sectionTraceSettings);
    traceArea(secStart);
}

bool PeTracer::traceFunction(offset_t offset, Executable::addr_type aType, QString name)
{
    const offset_t start = this->convertAddr(offset, aType, Executable::RAW);
    if (start == INVALID_ADDR) return false;

    if (!makeDisasmAt(m_Exe, start, functionTraceSettings)) {
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
