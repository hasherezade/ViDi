#pragma once

#include <QAbstractTableModel>
#include "OffsetedModel.h"

class CalledImportsModel : public OffsetedModel
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

    CalledImportsModel(QObject *v_parent)
        : OffsetedModel(v_parent, Executable::VA) { }

    virtual ~CalledImportsModel() { }

    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

       
    offset_t rowToOffset(const int row) const
    {
        if (row >= getTracer()->getImpFunctionsList().size()) {
            return INVALID_ADDR;
        }
        return getTracer()->getImpFunctionsList().at(row);
    }

    size_t countElements() const { return ( getTracer()) ? getTracer()->getImpFunctionsList().size() : 0; }
};

