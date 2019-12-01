#pragma once

#include <QAbstractTableModel>
#include "core/Executables.h"

#include <bearparser/core.h>

class InfoTableModel : public QAbstractTableModel
{
    Q_OBJECT

signals:
    void hookRequested(ExeHandler* exe);

public slots:
    void modelChanged() { reset(); }

public:
    enum COLS
    {
        COL_NAME = 0,
        COL_RAW_SIZE,
        COL_VIRTUAL_SIZE,
        COUNT_COL
    };
    InfoTableModel(QObject *v_parent)
        : QAbstractTableModel(v_parent), m_Exes(NULL) {}

    virtual ~InfoTableModel() { }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }
    int rowCount(const QModelIndex &parent) const { return countElements(); }

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &, const QVariant &, int);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
        //no index item pointer
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &index) const { return QModelIndex(); } // no parent

public slots:
    void setExecutables(Executables *exes)
    {
        if (this->m_Exes != NULL) {
            //disconnect old
            QObject::disconnect(this->m_Exes, SIGNAL(exeListChanged()), this, SLOT( onExeListChanged() ) );
        }
        this->m_Exes = exes;
        reset();
        if (this->m_Exes != NULL) {
            QObject::connect(this->m_Exes, SIGNAL(exeListChanged()), this, SLOT( onExeListChanged() ) );
        }
    }
protected slots:
    void onExeListChanged() { reset(); }

protected:

    void reset()
    {
        beginResetModel(); 
        endResetModel();
    }

    Executables* m_Exes;

    QVariant getDisplayData(int role, int attribute, ExeHandler *exeHndl) const;
    int countElements() const { return (m_Exes == NULL)? 0: m_Exes->size(); }
};
