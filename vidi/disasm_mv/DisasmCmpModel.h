#pragma once

#include "DisasmBlockModel.h"

class DisasmCmpModel : public DisasmBlockModel
{
    Q_OBJECT

public:
    enum COLS
    {
        COL_OFFSET= 0,
        COL_MATCH,
        COL_TARGET,
        COL_ICON,
        COL_CODE,
        COUNT_COL
    };
    DisasmCmpModel(QObject *v_parent)
        : DisasmBlockModel(v_parent) { }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;

    void setTwinModel(DisasmCmpModel* twin) 
    {
         m_Twin = twin;
         reset();
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

protected:
    enum COL_MATCH_STATE
    {
        MCOL_NONE = -1,
        MCOL_NO = 0,
        MCOL_YES = 1,
        COUNT_MCOL
    };

    COL_MATCH_STATE matchesTwin(const QModelIndex &index) const;

    QString matchToString(const COL_MATCH_STATE &match) const
    {
        switch(match) {
            case MCOL_NO: return "-";
            case MCOL_YES: return "+";
            case MCOL_NONE: return "";
        }
        return "";
    }

    bool containedInTwin(offset_t offset) const;

    DisasmCmpModel* m_Twin;
};

