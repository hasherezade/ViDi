#include "StringsModel.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

#define EMPTY_NAME "-"

QVariant StringsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_OFFSET : return addrTypeName();
        case COL_NAME: return "String";
        case COL_REFS: return "Refs";
    }
    return QVariant();
}

QVariant StringsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || this->countElements() == 0 || getTracer() == NULL) return QVariant();
    int section = index.column();
    int row = index.row();

    Executable::addr_type aType = Executable::RAW;
    const offset_t offset = rowToOffset(row);

    if (role == Qt::UserRole) {
        const offset_t offsetRVA = getTracer()->convertAddr(rowToOffset(row), aType, Executable::RVA);
        return qint64(offsetRVA);
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    switch (section) {
        case COL_OFFSET : return QString::number(convertAddr(offset, aType), 16);
        case COL_NAME:
        {
            return getTracer()->getReferedString(offset, Executable::RAW);
        }
        case COL_REFS: 
        {
            QSet<offset_t>* refs = getTracer()->refsTo(offset, aType);
            if (!refs) return "";
            return refs->size();
        }
    }
    return QVariant();
}

