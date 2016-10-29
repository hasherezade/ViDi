#pragma once

#include "DisasmBaseModel.h"

class DisasmBlockModel : public DisasmBaseModel
{
    Q_OBJECT
public:
    DisasmBlockModel(QObject *v_parent)
        : DisasmBaseModel(v_parent), m_Block(NULL) { }
    
    int rowToOffsetIndex(const int row) const
    {
        return row + m_startIndex;
    }

    virtual offset_t rowToOffset(const int row) const
    {
        int oIndex = rowToOffsetIndex(row);

        if (!m_Block || oIndex >= m_Block->offsets.size()) return INVALID_ADDR;
        offset_t offset = m_Block->offsets[oIndex];
        return offset;
    }

public slots:
    void setCodeBlock(CodeBlock *block, offset_t start = INVALID_ADDR);
    size_t calcStartIndex(CodeBlock *block, offset_t start);

protected:
    QVariant decoration(const QModelIndex &index) const;

    size_t countElements() const
    {
        return (m_Block == NULL) ? 0 : m_Block->offsets.size() - m_startIndex;
    }

    CodeBlock *m_Block;
    size_t m_startIndex;
};
