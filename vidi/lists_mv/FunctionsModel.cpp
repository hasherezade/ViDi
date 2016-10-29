#include "FunctionsModel.h"

#include <QFileDialog>
#include <QMessageBox>
 #include <QIcon>
#include "FunctionsModel.h"

#define EMPTY_NAME "-"

QVariant FunctionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_OFFSET : return addrTypeName();
        case COL_NAME: return "Name (Tag)";
        case COL_REFS: return "Refs";
    }
    return QVariant();
}

QVariant FunctionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || this->countElements() == 0 || getTracer() == NULL) return QVariant();
    int section = index.column();
    int row = index.row();

    Executable::addr_type aType = Executable::RVA;
    const offset_t offset = rowToOffset(row);

    if (role == Qt::UserRole) {
        return qint64(offset);
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    switch (section) {
        case COL_OFFSET : return QString::number(convertAddr(offset, aType), 16);
        case COL_NAME:
        {
            if (! getTracer()->hasName(offset, Executable::RVA)) return EMPTY_NAME;
            return getTracer()->getFunctionName(offset, aType);
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

Qt::ItemFlags FunctionsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    int section = index.column();

    if (section == COL_NAME) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool FunctionsModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    int section = index.column();
    int row = index.row();

    if (section == COL_NAME) {
        QString name = data.toString();
        if (name == EMPTY_NAME && name.length() > 0) return false;
        offset_t off = rowToOffset(row);
        m_ExeHandler->setFunctionName(off, Executable::RVA, name);
        reset();
        return true;
    }
    return false;
}

