#pragma once

#include <QAbstractTableModel>
#include "OffsetedModel.h"

class FunctionsModel : public OffsetedModel
{
    Q_OBJECT

public:
    enum COLS
    {
        COL_OFFSET = 0,
        COL_NAME,
        COL_REFS,
        COUNT_COL
    };

    FunctionsModel(QObject *v_parent)
        : OffsetedModel(v_parent, Executable::VA) { }

    virtual ~FunctionsModel() { }

    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &data, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    offset_t rowToOffset(const int row) const
    {
        if (row >= getTracer()->getFunctionsList().size()) {
            return INVALID_ADDR;
        }
        return getTracer()->getFunctionsList().at(row);
    }

    size_t countElements() const { return ( getTracer()) ? getTracer()->getFunctionsList().size() : 0; }
};

