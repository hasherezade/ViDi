#pragma once
#include <QtWidgets>
#include <bearparser/core.h>
#include "core/Executables.h"
#include "GuiUtil.h"

#include "ui_offsetdialog.h"

namespace Ui {
    class OffsetDialog;
}

class OffsetDialog : public QDialog
{
    Q_OBJECT

signals:
    void goToTarget(offset_t, Executable::addr_type);

public:
    OffsetDialog(QWidget *parent = 0)
        : QDialog(parent), 
        m_ExeHandler(NULL)
    {
        m_ui.setupUi(this);

        QStringList list;
        list << "Raw" << "RVA" << "VA";
        this->m_ui.addrTypeCombo->addItems(list);

        connect(m_ui.okCancel_buttons, SIGNAL(rejected()), this, SLOT(hide()));
        connect(m_ui.okCancel_buttons, SIGNAL(accepted()), this, SLOT(targetAccepted()));
        connect(m_ui.offsetLabel, SIGNAL(textChanged (const QString&)), this, SLOT( validateOffset() ));
        connect(m_ui.addrTypeCombo, SIGNAL(currentIndexChanged ( int ) ), this, SLOT( validateOffset() ));
    }

    void showEvent(QShowEvent *ev)
    {
        validateOffset();
        this->m_ui.offsetLabel->selectAll();
        ev->accept();
    }

public slots:

    virtual void setExecutable(ExeHandler* exeHndl)
    {
        this->m_ExeHandler = exeHndl;
        validateOffset();
    }

    void setTarget(offset_t offset, Executable::addr_type aType);

private slots:
    void validateOffset();
    void targetAccepted();

private:
    Executable::addr_type indexToAddrType(int index)
    {
        switch (index) {
            case 0: return Executable::RAW;
            case 1 : return Executable::RVA;
            case 2: return Executable::VA;
        }
        return Executable::NOT_ADDR;
    }

    Ui::OffsetDialog m_ui;
    ExeHandler* m_ExeHandler;
};
