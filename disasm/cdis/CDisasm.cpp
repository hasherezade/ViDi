#include "CDisasm.h"

using namespace minidis;

cs_mode toCSmode(Executable::exe_bits bitMode)
{
    switch (bitMode) {
    case Executable::BITS_16:
        return CS_MODE_16;
    case Executable::BITS_32:
        return CS_MODE_32;
    case Executable::BITS_64:
        return CS_MODE_64;
    }
    return CS_MODE_32; //Default
}

bool CDisasm::init_capstone(Executable::exe_bits bitMode)
{
    cs_err err;
    err = cs_open(CS_ARCH_X86, toCSmode(bitMode), &handle);
    if (err) {
        printf("Failed on cs_open() with error returned: %u\n", err);
        return false;
    }
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    cs_option(handle, CS_OPT_SKIPDATA, CS_OPT_ON);
    m_insn = cs_malloc(handle);
    if (!m_insn) {
        cs_close(&handle);
        return false;
    }
    return true;
}

size_t CDisasm::capstone_next(const BYTE *code, size_t size, offset_t startRaw)
{
    bool isOk = cs_disasm_iter(handle, &code, &size, &startRaw, m_insn);
    //m_csCount = cs_disasm(handle, code, size, startRaw, 1, &insn);
    if (!isOk || !m_insn) {
        return 0;
    }
    //printf("Disasembled: %d\t %s %s\t%lx\n", m_csCount, insn[0].mnemonic, insn[0].op_str, insn[0].address);
    return m_insn->size;
}

bool CDisasm::init(const offset_t raw, const bufsize_t disasmSize, Executable::exe_bits bitMode)
{
    m_startOffset = raw;
    m_bitMode = bitMode;
    m_disasmBuf.clear();

    //printf("CDisasm::init : Init buffer at offset %lx bit mode: %d\n", m_startOffset, m_bitMode);
    m_buf = m_Exe->getContentAt(m_startOffset, Executable::RAW);
    m_bufSize = m_Exe->getContentSize() - m_startOffset;

    if (m_buf == NULL || m_bufSize == 0) return false;

    m_disasmSize = m_bufSize < disasmSize ? m_bufSize : disasmSize;
    m_disasmBuf.setStartOffset(m_startOffset);

    m_csBuf = m_buf;
    m_csBufSize = m_disasmSize;
    m_csOffset = 0;

    this->is_init = init_capstone(m_bitMode);
    return this->is_init;
}

size_t CDisasm::disasmNext()
{
    if (!is_init && m_insn) {
        printf("Cannot disasm next = NOT INIT!\n");
        return 0;
    }
    //bool cs_disasm_iter(csh ud, const uint8_t **code, size_t *size,
    size_t step = capstone_next(m_csBuf, m_csBufSize, m_csOffset);
    if (step == 0) {
        is_init = false;
        return 0;
    }
    m_csBuf += step;
    m_csBufSize -= step;
    m_csOffset += step;
    return step;
}

DisasmChunk* CDisasm::makeChunk(offset_t startRVA)
{
    if (!m_insn) {
        return NULL;
    }
    return new CapstoneChunk(*m_insn, startRVA, m_Exe);
}
