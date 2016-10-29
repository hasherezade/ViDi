#include "DisasmCmpModel.h"

QVariant DisasmCmpModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_OFFSET : return addrTypeName();
        case COL_MATCH : return "L=R";
        case COL_CODE: return "Code";
    }
    return QVariant();
}

DisasmCmpModel::COL_MATCH_STATE DisasmCmpModel::matchesTwin(const QModelIndex &index) const
{
    if (!index.isValid()) return MCOL_NONE;
    if (m_Twin == NULL) return MCOL_NONE;
    offset_t myOffset = rowToOffset(index.row());
    offset_t twinOffset = m_Twin->rowToOffset(index.row());
    if (twinOffset == INVALID_ADDR) return MCOL_NONE;

    QString myStr = getTracer()->getDisasmString(myOffset, Executable::RAW);
    QString twinStr = getTracer()->getDisasmString(twinOffset, Executable::RAW);
    int cmp = QString::compare(myStr, twinStr, Qt::CaseInsensitive); 
    if (cmp == 0) return MCOL_YES;
    return MCOL_NO;
}

bool DisasmCmpModel::containedInTwin(offset_t offset) const
{
    const offset_t twinStart = m_Twin->rowToOffset(0);
    if (twinStart == INVALID_ADDR) return false;

    if (offset >= twinStart &&  offset <= m_Twin->m_Block->getEndOffset()) {
        return true;
    }
    return false;
}

Qt::ItemFlags DisasmCmpModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    int row = index.row();
    offset_t offset = rowToOffset(row);
    if (offset < this->m_startRaw) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant DisasmCmpModel::data(const QModelIndex &index, int role) const
{
    if (this->countElements() == 0 || getTracer() == NULL) return QVariant();
    if (!m_Block || index.row() >= m_Block->offsets.size() ) return QVariant();

    offset_t end = m_Block->getEndOffset();

    int attribute = index.column();
    int row = index.row();
    const offset_t offset = rowToOffset(row);

    const offset_t target = getTargetOffset(index);
    if (role == Qt::UserRole) {
        return qint64(offset);
    }

    if (role == Qt::TextColorRole) {
        return textColor(index);
    }
    if (role == Qt::BackgroundColorRole) {
        if (attribute == COL_OFFSET && this->containedInTwin(offset)) return QColor("blue");
        return backgroundColor(index);
    }
    if (role == Qt::DecorationRole && attribute == COL_ICON) {
        return decoration(index);
    }
    if (role == Qt::ToolTipRole) {
        if (isHoovered(index)) return "DoubleClick to follow";
    }
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole) return QVariant();
    
    switch (attribute) {
        case COL_OFFSET :
            return this->getNamedOffset(offset);
        case COL_CODE :
        {
            return getTracer()->getDisasmString(offset, Executable::RAW);
        }
        case COL_MATCH :
        {
            return matchToString(matchesTwin(index));

        }
        case COL_TARGET :
        {
            if ( getTracer()->isBranching(offset, Executable::RAW)) return QVariant();
            if (! getTracer()->hasReferedString(target, Executable::RAW)) return QVariant();
            return getTracer()->getReferedString(target, Executable::RAW);
        }
    }
    return QVariant();
}
