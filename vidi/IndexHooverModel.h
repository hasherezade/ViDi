#pragma once

#include <QAbstractTableModel>

#ifndef INVALID_INDEX
    #define INVALID_INDEX (-1)
#endif

class IndexHooverModel : public QObject
{
    Q_OBJECT

signals:
    void hooverStateChanged() const;

public:
    IndexHooverModel(QAbstractTableModel *v_parent)
        : QObject(v_parent), m_parentTableModel(v_parent),
        m_hooveredX(INVALID_INDEX), m_hooveredY(INVALID_INDEX) {}

    virtual ~IndexHooverModel() {}
    
    bool isHoovered(const QModelIndex &index) const
    {
        if (!index.isValid()) {
            return false;
        }
        if (index.row() == m_hooveredY && index.column() == m_hooveredX) {
            return true;
        }
        return false;
    }

public slots:
    void unhooverIndex()
    {
        if (m_hooveredX == INVALID_INDEX && m_hooveredY == INVALID_INDEX) return;
        m_hooveredX = INVALID_INDEX;
        m_hooveredY = INVALID_INDEX;
        emit hooverStateChanged();
        //reset();
        return;
    }

    void hooverIndex(const QModelIndex &index)
    {
        if (!index.isValid()) {
            unhooverIndex();
            return;
        }
        if (m_hooveredX == index.row() && m_hooveredY == index.column()) return;
        m_hooveredY = index.row();
        m_hooveredX = index.column();
        emit hooverStateChanged();
    }

protected:
    QAbstractTableModel *m_parentTableModel;
    int m_hooveredX, m_hooveredY;
};
