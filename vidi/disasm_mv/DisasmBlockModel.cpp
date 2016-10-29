#include "DisasmBlockModel.h"

void DisasmBlockModel::setCodeBlock(CodeBlock *block, offset_t start)
{
    m_Block = block;
    m_startIndex = calcStartIndex(block, start);
    if (!block || start == INVALID_ADDR || m_startIndex == INVALID_INDEX) {
        m_startIndex = 0;
        setCodeArea(INVALID_ADDR, INVALID_ADDR);
        return;
    }
    setCodeArea(start, block->getEndOffset());
}

size_t DisasmBlockModel::calcStartIndex(CodeBlock *block, offset_t start)
{
    if (!block) return INVALID_INDEX;
    if (block->start == start) return 0;
    return block->getIndexOf(start);
}

//---

QVariant DisasmBlockModel::decoration(const QModelIndex &index) const
{
    const offset_t current = indexToOffset(index);
    const offset_t target = getTargetOffset(index);
    if (current == target) {
        return this->m_iconSet.tracerSelf;
    }
    if (m_Block) {
        const offset_t end = m_Block->getEndOffset();
        ForkPoint *fork = getTracer()->forkAt(end);
        
        if (fork && m_Block->contains(fork->yesOffset)) {
            if (current == fork->yesOffset) {
                return this->m_iconSet.tracerDownIcon;
            }
            if (current > fork->yesOffset && target != fork->yesOffset) {
                return this->m_iconSet.tracerIcon;
            }
        }
    }
    return DisasmBaseModel::decoration(index);
}

