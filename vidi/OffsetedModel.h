#pragma once

#include <QAbstractTableModel>
#include "ExeDependentModel.h"
#include "IndexHooverModel.h"
#include "ReferenceSelectModel.h"

class OffsetedModel : public ExeDependentModel
{
    Q_OBJECT

public slots:
    void onSetAddrType(Executable::addr_type aType)
    {
        this->m_addrType = aType;
        reset();
    }

public:
    OffsetedModel(QObject *v_parent, Executable::addr_type addrType)
        : ExeDependentModel(v_parent), m_addrType(addrType),
        m_hooverModel(this)
    {
        connect(&m_hooverModel, SIGNAL(hooverStateChanged()), this, SLOT(onStateChanged()));
    }

    IndexHooverModel* getHooverModel() { return &m_hooverModel; }

protected:
    /* data */
    offset_t convertAddr(offset_t offset, Executable::addr_type inType) const
    {
        if (!this-> getTracer()) return INVALID_ADDR;
        return getTracer()->convertAddr(offset, inType, m_addrType);
    }

    QString addrTypeName() const{
        switch (m_addrType) {
            case Executable::RAW : return "Raw";
            case Executable::RVA : return "RVA";
            case Executable::VA : return "VA";
        }
        return "";
    }

    const QString getNameOfRefered() const
    {
        if (!m_refSelectModel) return "";
        return m_refSelectModel->getNameOfRefered();
    }

    Executable::addr_type m_addrType;
    IndexHooverModel m_hooverModel;
};

