#pragma once

#include <bearparser.h>
#include "DisasmChunk.h"

namespace minidis {
//---
class DisasmChunkBuf
{
public:
    DisasmChunkBuf() : m_offset(0) {}
    ~DisasmChunkBuf() {}

    const DisasmChunk* objAt(const size_t index) const
    {
        return (m_table.size() > index) ? m_table.at(index) : NULL;
    }

    size_t size() const { return m_table.size(); }

    DisasmChunk* at(size_t index) const
    {
        if (index >= m_table.size()) {
            return NULL;
        }
        return m_table.at(index);
    }

    void clear() { m_table.clear(); }
    void append(DisasmChunk *chunk);

    void setStartOffset(offset_t offset) { m_offset = offset; }
    offset_t getStartOffset() const { return m_offset; }

    offset_t indexToOffset(const size_t index) const;
    size_t offsetToIndex(const offset_t raw) const;

    bufsize_t getChunkSize(const size_t index) const;
    QString getDisasmString(const size_t index);
    QString getMnemonicString(const size_t index);

protected:
    offset_t m_offset; // buffer start offset
    std::vector<DisasmChunk*> m_table;
    std::map<offset_t, size_t> m_offsetToIndex; // RAW -> index
};

//---
}; // namespace minidis
