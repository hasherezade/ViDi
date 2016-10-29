#pragma once

#include <QAbstractTableModel>
#include <QMenu>
#include "DisasmBlockModel.h"

class DisasmModel : public DisasmBlockModel
{
    Q_OBJECT

public:
    enum COLS
    {
        COL_OFFSET= 0,
        COL_NAME,
        COL_REFS,
        COL_TARGET,
        COL_ICON,
        COL_CODE,
        COL_HEX,
        COUNT_COL
    };
    DisasmModel(QObject *v_parent)
        : DisasmBlockModel(v_parent){ }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &data, int role);

    QString refsToString(QSet<offset_t>* refs) const
    {
        if (!refs) return "";
        QSet<offset_t>::iterator itr;
        QStringList list;
        for (itr = refs->begin(); itr!= refs->end(); itr++) {
            offset_t addr = getTracer()->convertAddr(*itr, Executable::RAW, m_addrType);
            list << getTracer()->getFunctionName(addr, m_addrType);
        }
        return list.join("\n");
    }

    virtual bool isClickable(const QModelIndex &index) const
    {
        if (!index.isValid()) return false;
        const int col = index.column();
        switch (col) {
            case COL_OFFSET:
                return true;
            case COL_ICON:
            case COL_CODE:
                return isTargetFollowable(index);
        }
        return false;
    }
    
};
