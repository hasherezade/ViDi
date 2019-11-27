#pragma once

#include <QAbstractTableModel>
#include <QMenu>

#include "../ExeDependentModel.h"
#include "../IndexHooverModel.h"

#include "DisasmIconSet.h"
#include "DisasmColorSet.h"

class DisasmBaseModel : public ExeDependentModel
{
    Q_OBJECT

public:
    DisasmBaseModel(QObject *v_parent)
        : ExeDependentModel(v_parent),
            m_startRaw(INVALID_ADDR), m_endRaw(INVALID_ADDR),
            m_addrType(Executable::RVA),
            m_hooverModel(this)
    {
        connect(&m_hooverModel, SIGNAL(hooverStateChanged()), this, SLOT(onStateChanged()));
    }
    
    virtual ~DisasmBaseModel() { }
//---

    IndexHooverModel* getHooverModel() { return &m_hooverModel; }

    virtual offset_t rowToOffset(const int row) const = 0;

    virtual offset_t indexToOffset(const QModelIndex &index) const
    {
        if (index.isValid() == false) return INVALID_ADDR;
        if (this->countElements() == 0 || getTracer() == NULL) return INVALID_ADDR;
        return rowToOffset(index.row());
    }

    void setColors(const DisasmColorSet &colorSet) { m_colorSet = colorSet; reset(); }

    // for DisasmView
    offset_t getOffsetToFollow(const QModelIndex &index, Executable::addr_type outType = Executable::RAW)
    {
        if (! getTracer()) return INVALID_ADDR;
        if (index.column() == 0) { //offset column
            const offset_t current = indexToOffset(index);
            return getTracer()->convertAddr(current, Executable::RAW, outType);
        }
        const offset_t target = getTargetOffset(index, outType);
        return target;
    }

    virtual bool isClickable(const QModelIndex &index) const { return false; }

    //overwrite: ExeDependentModel
    virtual void setReferenceSelectionModel(ReferenceSelectModel *refSelectModel)
    {
        if (m_refSelectModel == refSelectModel) return; //nothing changed

        if (m_refSelectModel) {
            disconnect(m_refSelectModel, SIGNAL(selectedReferencesChanged()), this, SLOT(onStateChanged()));
        }
        m_refSelectModel = refSelectModel;
        if (m_refSelectModel) {
            connect(m_refSelectModel, SIGNAL(selectedReferencesChanged()), this, SLOT(onStateChanged()));
        }
    }

public slots:
    void setCodeArea(offset_t start, offset_t end)
    {
        m_startRaw = start;
        m_endRaw = end;
        reset();
    }

protected:
    //wrappers
    offset_t getTargetOffset(const QModelIndex &index, const Executable::addr_type outType = Executable::RAW) const
    {
        if (! getTracer()) {
            return INVALID_ADDR;
        }
        const Executable::addr_type inType = Executable::RAW;
        return getTracer()->getTargetOffset(indexToOffset(index), inType, outType);
    }

    offset_t convertAddr(const offset_t inAddr, Executable::addr_type inType, Executable::addr_type outType) const
    {
        if (!m_ExeHandler || !m_ExeHandler->getExe()) {
            return INVALID_ADDR;
        }
        return m_ExeHandler->getExe()->convertAddr(inAddr, inType, outType);
    }

    //---
    bool isTargetFollowable(const QModelIndex &index) const;

    /* data */
    offset_t convertAddr(offset_t offset) const;
    QString addrTypeName() const;
    virtual size_t countElements() const = 0;
    
    minidis::mnem_type getMnemTypeAt(const QModelIndex &index) const
    {
        if (! getTracer()) return MT_OTHER;
        return getTracer()->getMnemTypeAtOffset(indexToOffset(index), Executable::RAW);
    }

    QString getFormatedOffset(offset_t offset) const;
    QString getNamedOffset(offset_t offset) const;

    /* view */
    virtual QVariant textColor(const QModelIndex &index) const;
    virtual QVariant backgroundColor(const QModelIndex &index) const;
    virtual QVariant decoration(const QModelIndex &index) const;

    bool isHoovered(const QModelIndex &index) const
    {
        return m_hooverModel.isHoovered(index);
    }

    offset_t m_startRaw, m_endRaw;
    Executable::addr_type m_addrType;

    DisasmColorSet m_colorSet;
    DisasmIconSet m_iconSet;

    IndexHooverModel m_hooverModel;
};
