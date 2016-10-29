#include "UDisasm.h"

using namespace minidis;

bool UDisasm::init(offset_t raw, bufsize_t disasmSize, Executable::exe_bits bitMode)
{
    m_startOffset = raw;
    m_bitMode = bitMode;
    m_disasmBuf.clear();

    //printf("UDisasm::init : Init buffer at offset %lx bit mode: %d\n", m_startOffset, m_bitMode);
    m_buf = m_Exe->getContentAt(m_startOffset, Executable::RAW);
    m_bufSize = m_Exe->getContentSize() - m_startOffset;
    if (m_buf == NULL || m_bufSize == 0) return false;

    m_iptr = 0;

    m_disasmSize = m_bufSize < disasmSize ? m_bufSize : disasmSize;
    m_disasmBuf.setStartOffset(m_startOffset);

    ud_init(&this->ud_obj);
    ud_set_input_buffer(&this->ud_obj, m_buf, m_disasmSize);
    ud_set_mode(&this->ud_obj, static_cast<uint8_t>(m_bitMode));
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);

    this->is_init = true;
    return true;
}

size_t UDisasm::disasmNext()
{
    if (!is_init) {
        printf("Cannot disasm next = NOT INIT!\n");
        return 0;
    }
    if (!ud_disassemble(&this->ud_obj)) {
        is_init = false;
        return 0;
    }
    size_t step =  ud_insn_len(&this->ud_obj);
    m_iptr += step;
    return step;
}

bool UDisasm::fillTable(bool stopAtBlockEnd, size_t max_el)
{
    //printf("UDisasm::fillTable\n");
    if (this->is_init == false) {
        printf("Not initialized!\n");
        return false;
    }
    
    size_t index = 0;
    offset_t startRVA = this->convertAddr(m_startOffset, Executable::RAW, Executable::RVA);

    for (index = 0; disasmNext() > 0; index ++) {
        UdisChunk *uChunk = new  UdisChunk(ud_obj, startRVA, m_Exe);
        if (!uChunk) break;

        m_disasmBuf.append(uChunk);
        //if (index < 2) printf("+%s\n", uChunk->toString().toStdString().c_str());
        if (m_disasmBuf.size() == max_el) stopAtBlockEnd = true;

        if (m_disasmBuf.indexToOffset(index) == INVALID_ADDR) break;
        if (stopAtBlockEnd && uChunk->isBlockEnd()) {
            //printf("+%s\n", uChunk->toString().toStdString().c_str());
            break;
        }
    }
    return true;
}

/* TODO: test it!!! */
int64_t UDisasm::getSignedLVal(const ud_t &inpObj, size_t argNum, bool &isOk) const
{
    if (argNum > MAX_ARG_NUM) return INVALID_ADDR;
    isOk = false;
    int64_t lValue = 0;
    uint8_t size = inpObj.operand[argNum].size;

    if (size == 0) return 0;
    uint8_t maxSize = sizeof(uint64_t) * 8; // in bits
    uint8_t dif = maxSize - size;

    lValue = inpObj.operand[argNum].lval.uqword;
    int64_t mlValue = (lValue << dif) >> dif; // gives signed!!!
    isOk = true;
    return mlValue;
}

