#include "CalledImportsModel.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

#define EMPTY_NAME "-"

QVariant CalledImportsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_OFFSET : return addrTypeName();
        case COL_NAME: return "Import Name";
        case COL_REFS: return "Refs";
    }
    return QVariant();
}

QVariant CalledImportsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || this->countElements() == 0 || getTracer() == NULL) return QVariant();
    int section = index.column();
    int row = index.row();
    const Executable::addr_type aType = Executable::RVA;
    const offset_t offset = rowToOffset(row);

    if (role == Qt::UserRole) {
        return qint64(offset);
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    switch (section) {
        case COL_OFFSET : return QString::number(convertAddr(offset, Executable::RVA), 16);
        case COL_NAME:
        {
            return getTracer()->getImportName(offset, Executable::RVA);
        }
        case COL_REFS:
        {
            QSet<offset_t>* refs = getTracer()->refsTo(offset, Executable::RVA);
            if (!refs) return "";
            return refs->size();
        }
    }
    return QVariant();
}

