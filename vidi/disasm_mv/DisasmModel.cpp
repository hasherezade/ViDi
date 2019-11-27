#include "DisasmModel.h"

#include <QFileDialog>
#include <QMessageBox>

#define YES_COLOR "lime"
#define NO_COLOR "red"
#define EMPTY_NAME ""

QVariant DisasmModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_OFFSET : return addrTypeName();
        case COL_NAME : return "Tag";
        case COL_REFS: return "Refs";
        case COL_CODE: return "Code";
        case COL_HEX: return "Hex";
        case COL_TARGET : return "Target";
    }
    return QVariant();
}

QVariant DisasmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return false;
    if (this->countElements() == 0 || getTracer() == NULL) return QVariant();

    if (!m_Block || index.row() >= m_Block->offsets.size() ) return QVariant();
    
    offset_t end = m_Block->getEndOffset();
    ForkPoint *fork = getTracer()->forkAt(m_Block->getEndOffset());

    int attribute = index.column();
    int row = index.row();
    const offset_t offset = this->rowToOffset(row);
    const Executable::addr_type aType = Executable::RAW;

    const offset_t targetRaw = getTracer()->getTargetOffset(offset, Executable::RAW, Executable::RAW);
    if (role == Qt::UserRole) {
        return qint64(offset);
    }

    if (role == Qt::ToolTipRole) {
        if (isHoovered(index)) {
            return "DoubleClick to follow";
        }
        if (attribute == COL_REFS) {
            QSet<offset_t>* refs = getTracer()->refsTo(offset, aType);
            return refsToString(refs);
        }
    }
    if (role == Qt::TextColorRole) {
        return textColor(index);
    }
    if (role == Qt::BackgroundColorRole) {
        return backgroundColor(index);
    }
    if (role == Qt::DecorationRole && attribute == COL_ICON) {
        return decoration(index);
    }
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole) return QVariant();
    
    switch (attribute) {
        case COL_OFFSET :
            return getFormatedOffset(offset);
        case COL_HEX:
        {
            return getTracer()->getHexString(offset, Executable::RAW);
        }
        case COL_CODE :
        {
            return getTracer()->getDisasmString(offset, Executable::RAW);
        }
        case COL_NAME :
        {
            if ( getTracer()->hasName(offset, Executable::RAW)) {
                return getTracer()->getFunctionName(offset, Executable::RAW);
            }
            return EMPTY_NAME;
        }
        case COL_REFS:
        {
            QSet<offset_t>* refs = getTracer()->refsTo(offset, Executable::RAW);
            if (!refs) return "";
            return refs->size();
        }
        case COL_TARGET :
        {
            if ( getTracer()->isBranching(offset, Executable::RAW)) return QVariant();

            const offset_t target = getTracer()->getTargetOffset(offset, Executable::RAW, Executable::RAW);
            const offset_t targetRVA = getTracer()->getTargetOffset(offset, Executable::RAW, Executable::RVA);

            //offset_t convRVA = this->m_ExeHandler->getExe()->convertAddr(target, Executable::RAW, Executable::RVA);
            if (target == INVALID_ADDR && targetRVA == INVALID_ADDR) {
                return QVariant();
            }
 
            if (! getTracer()->hasReferedString(target, Executable::RAW)) return QVariant();
            return getTracer()->getReferedString(target, Executable::RAW);
        }
    }
    return QVariant();
}

Qt::ItemFlags DisasmModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    int section = index.column();

    if (section == COL_NAME) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool DisasmModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    int section = index.column();
    int row = index.row();

    if (section == COL_NAME) {
        QString name = data.toString();
        if (name == EMPTY_NAME && name.length() > 0) return false;
        offset_t off = this->rowToOffset(row);
        m_ExeHandler->setFunctionName(off, Executable::RAW, name);
        reset();
        return true;
    }
    return false;
}
