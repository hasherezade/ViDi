#include "DisasmChunkBuf.h"

using namespace minidis;

void DisasmChunkBuf::append(DisasmChunk* chunk)
{
    if (!chunk) return;

    const size_t index = m_table.size();
    const offset_t offset = chunk->getOffset();

    m_table.push_back(chunk);
    m_offsetToIndex[offset] = index;
}

offset_t DisasmChunkBuf::indexToOffset(const size_t index) const
{
    DisasmChunk* chunk = at(index);
    if (!chunk) {
        return INVALID_ADDR;
    }
    offset_t offset = chunk->getOffset();
    //printf("offset = %x m_offset: %x\n", offset, m_offset);
    offset += m_offset;
    return offset;
}

size_t DisasmChunkBuf::offsetToIndex(const offset_t raw) const
{
    if (raw < m_offset) {
        return INVALID_INDEX;
    }
    offset_t offset = raw - m_offset;
    std::map<offset_t, size_t>::const_iterator itr = m_offsetToIndex.find(offset);
    if (itr == m_offsetToIndex.end()) {
        return INVALID_INDEX;
    }
    size_t index = itr->second;
    return index;
}

QString DisasmChunkBuf::getDisasmString(const size_t index)
{
    DisasmChunk* chunk = at(index);
    if (!chunk) return "";

    return chunk->toString();
}


QString DisasmChunkBuf::getMnemonicString(const size_t index)
{
    DisasmChunk* chunk = at(index);
    if (!chunk) return "";

    return chunk->getMnemString();
}

bufsize_t DisasmChunkBuf::getChunkSize(const size_t index) const
{
    DisasmChunk* chunk = at(index);
    if (!chunk) return 0;

    return chunk->getChunkSize();
}
