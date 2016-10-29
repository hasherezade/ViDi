#pragma once

#include <QAbstractTableModel>
#include "OffsetedModel.h"

class StringsModel : public OffsetedModel
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

    StringsModel(QObject *v_parent)
        : OffsetedModel(v_parent, Executable::VA) { }

    virtual ~StringsModel() { }

    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    offset_t rowToOffset(const int row) const
    {
        if (row >= getTracer()->getReferedStringsList().size()) {
            return INVALID_ADDR;
        }
        return getTracer()->getReferedStringsList().at(row);
    }

    size_t countElements() const { return ( getTracer()) ? getTracer()->getReferedStringsList().size() : 0; }
};

