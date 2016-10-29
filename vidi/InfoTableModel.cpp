#include "InfoTableModel.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

#define HIGLIHHT_COLOR "yellow"
#define OK_COLOR "green"
#define NOT_OK_COLOR "red"

QVariant InfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_NAME : return "FileName";
        case COL_RAW_SIZE: return "RawSize";
        case COL_VIRTUAL_SIZE: return "VirtualSize";
    }
    return QVariant();
}

Qt::ItemFlags InfoTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return f;
}

bool InfoTableModel::setData(const QModelIndex &index, const QVariant &, int role)
{
    return false;
}


QVariant InfoTableModel::data(const QModelIndex &index, int role) const
{
    int elNum = index.row();
    if (elNum > countElements()) return QVariant();

    int attribute = index.column();
    if (attribute >= COUNT_COL) return QVariant();

    if (role == Qt::UserRole) {
        return int(elNum);
    }
    ExeHandler *exeHndl = m_Exes->at(elNum);
    if (exeHndl == NULL) return QVariant();
    Executable *exe = exeHndl->getExe();
    if (exe == NULL) return QVariant();

    if (role == Qt::DisplayRole) {
        return getDisplayData(role, attribute, exeHndl);
    }

    if (role == Qt::DecorationRole && attribute == COL_NAME) {
        if (exe->isBit32()) return QIcon(":/icons/app32.ico");
        if (exe->isBit64()) return QIcon(":/icons/app64.ico");
        if (exe->getBitMode() == Executable::BITS_16) return QIcon(":/icons/dos_exe.ico");
        return QVariant();
    }

    PEFile *pe = dynamic_cast<PEFile*>(exeHndl->getExe());
    if (pe == NULL) return QVariant();

    if (role == Qt::TextColorRole ) {
        if (exeHndl->isTruncated()) return QColor("red");
        return QVariant();
    }

    if (role == Qt::ToolTipRole ) {
        return "";
    }
    return QVariant();
}

QVariant InfoTableModel::getDisplayData(int role, int attribute, ExeHandler *exeHndl) const
{
    Executable *exe = exeHndl->getExe();
    if (exe == NULL) return QVariant();

    if (role != Qt::DisplayRole) return QVariant();

    switch (attribute) {
        case COL_NAME :
        {
            
            QFileInfo inputInfo(exeHndl->getFileName());
            QString name = inputInfo.fileName();
            if (exeHndl->isTruncated()) name += " (truncated)";
            return name;
        }
        case COL_RAW_SIZE : return QString::number(exe->getMappedSize(Executable::RAW), 16);
        case COL_VIRTUAL_SIZE : return QString::number(exe->getMappedSize(Executable::RVA), 16);

    }
    return QVariant();
}

