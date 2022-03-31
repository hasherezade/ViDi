#include "OffsetDialog.h"

void OffsetDialog::validateOffset()
{
    if (!m_ExeHandler || !m_ExeHandler->getExe()) return;
    
    Executable *my_exe = m_ExeHandler->getExe();
    
    QString text = m_ui.offsetLabel->text();
    offset_t number = 0;
    if (text.length() > 0) {
        bool isValid = false;
        number = text.toULongLong(&isValid, 16);
        if (!isValid) {
            m_ui.offsetLabel->setStyleSheet("background-color : rgba(255,0,0,100);");
            m_ui.eqTypeLabel->setText("(invalid)");
            m_ui.eqLabel->setText("invalid format");
            return;
        }
    }
    Executable::addr_type aType = indexToAddrType(m_ui.addrTypeCombo->currentIndex());
    if (aType == Executable::NOT_ADDR) return;

    Executable::addr_type otherType = aType == Executable::RAW ? Executable::RVA : Executable::RAW;
    if (!my_exe->isValidAddr(number, aType) ) {
        m_ui.offsetLabel->setStyleSheet("background-color : rgba(255,0,0,100);");
        m_ui.eqTypeLabel->setText("(invalid)");
        m_ui.eqLabel->setText("out of scope");
        return;
    }
    m_ui.offsetLabel->setStyleSheet("background-color : white;");
    offset_t otherAddr = my_exe->convertAddr(number, aType, otherType);
    m_ui.eqTypeLabel->setText(vidi::translateAddrTypeName(otherType) + ":");
    if (otherAddr == INVALID_ADDR) {
        m_ui.eqLabel->setText("cannot map");
    } else {
        m_ui.eqLabel->setText(QString::number(otherAddr, 16));
    }
}

void OffsetDialog::targetAccepted()
{
    QString text = m_ui.offsetLabel->text();
    if (text.length() == 0) return;

    bool isValid = false;
    offset_t number = text.toULongLong(&isValid, 16);
    if (!isValid) return;

    Executable::addr_type aType = indexToAddrType(m_ui.addrTypeCombo->currentIndex());
    if (aType == Executable::NOT_ADDR) return;

    emit goToTarget(number, aType);
}

void OffsetDialog::setTarget(offset_t target, Executable::addr_type inType)
{
    if (!m_ExeHandler) return;

    Executable::addr_type aType = indexToAddrType(m_ui.addrTypeCombo->currentIndex());
    offset_t offset = m_ExeHandler->getExe()->convertAddr(target, inType, aType);

    m_ui.offsetLabel->setText(QString::number(offset, 16));
}
