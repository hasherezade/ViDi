#pragma once

#include <QAbstractTableModel>
#include "core/Executables.h"
#include "ReferenceSelectModel.h"

#ifndef INVALID_INDEX
    #define INVALID_INDEX (-1)
#endif
class ExeDependentModel : public QAbstractTableModel
{
    Q_OBJECT

signals:
    void resetView();

public:
    ExeDependentModel(QObject *v_parent)
        : QAbstractTableModel(v_parent),
        m_ExeHandler(NULL), m_refSelectModel(NULL)
    { }
    virtual ~ExeDependentModel() { }

    int rowCount(const QModelIndex &parent) const { return countElements(); }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {
        //no index item pointer
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &index) const { return QModelIndex(); } // no parent

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if (!index.isValid()) return 0;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    bool setData(const QModelIndex &index, const QVariant &data, int role)
    {
        return false;
    }

    void reset()
    {
        QAbstractTableModel::reset();
        emit resetView();
    }

    virtual void setReferenceSelectionModel(ReferenceSelectModel *refSelectModel)
    {
        if (m_refSelectModel == refSelectModel) return; //nothing changed
        m_refSelectModel = refSelectModel;
    }

    virtual size_t countElements() const = 0;

public slots:
    void modelChanged() { reset(); }

    virtual void setExecutable(ExeHandler* exeHndl)
    {
        if (exeHndl == m_ExeHandler) return; //nothing to change

        if (m_ExeHandler) {
            disconnect(m_ExeHandler, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
        }
        this->m_ExeHandler = exeHndl;

        if (this->m_ExeHandler) {
            connect(m_ExeHandler, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
        }
        reset();
    }

protected slots:
    void onExeChanged() { reset(); }
    void onStateChanged() { reset(); }

protected:
    Tracer* getTracer() const
    {
        return (m_ExeHandler) ? m_ExeHandler->getTracer() : NULL;
    }

    ExeHandler* m_ExeHandler;
    ReferenceSelectModel *m_refSelectModel;
};
