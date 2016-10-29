#include "DisasmBaseModel.h"

#include "../GuiUtil.h"

 offset_t DisasmBaseModel::convertAddr(offset_t offset) const
{
    if (!this-> getTracer()) return INVALID_ADDR;
    return getTracer()->convertAddr(offset, Executable::RAW, m_addrType);
}

QString DisasmBaseModel::addrTypeName() const
{
    return vidi::translateAddrTypeName(m_addrType);
}

bool DisasmBaseModel::isTargetFollowable(const QModelIndex &index) const
{
    const minidis::mnem_type mType = getMnemTypeAt(index);
    if (mType == MT_INTX) return false;

    return getTargetOffset(index) != INVALID_ADDR;
}

QVariant DisasmBaseModel::textColor(const QModelIndex &index) const
{
    const minidis::mnem_type mType = getMnemTypeAt(index);
    switch (mType) {
        case MT_INTX : return m_colorSet.importColor;
        case MT_RET : return m_colorSet.retColor;
        case MT_NOP : return m_colorSet.nopColor;
        case MT_INT3 : return m_colorSet.int3Color;
        case MT_INVALID : return m_colorSet.invalidColor;
        //default: return QVariant();//return m_colorSet.basicTextColor;
    }

    const offset_t offset = indexToOffset(index);
    if ( getTracer()->isInternalCall(offset , Executable::RAW)) {
        return m_colorSet.internalCallColor;
    }
    const offset_t target = getTargetOffset(index);
    if (target != INVALID_ADDR && getTracer()->isImportedFunction(target, Executable::RAW)) {
        return m_colorSet.importColor;
    }
    return QVariant();
}

QVariant DisasmBaseModel::backgroundColor(const QModelIndex &index) const
{
    const offset_t current = indexToOffset(index);
    offset_t target = getTargetOffset(index);
        if (isHoovered(index)) return m_colorSet.clickableColor;

    if (this->m_refSelectModel) {
        if (m_refSelectModel->isOffsetRefered(current,Executable::RAW)) return m_colorSet.referedColor;
        if (m_refSelectModel->isOffsetRefered(target,Executable::RAW)) return m_colorSet.referingColor;
    }
    if (isTargetFollowable(index)) return m_colorSet.branchingColor;
    return QVariant();
}

QVariant DisasmBaseModel::decoration(const QModelIndex &index) const
{
    const minidis::mnem_type mType = getMnemTypeAt(index);
    if (mType == MT_INTX) {
        //no decoration for interrupts
        return QVariant(); 
    }
    const offset_t target = getTargetOffset(index);
    const offset_t targetVA = getTargetOffset(index, Executable::VA);

    if (target == INVALID_ADDR && targetVA == INVALID_ADDR) {
        //target does not exist
        return QVariant();
    }
    if (target == INVALID_ADDR) {
        //address cannot be converted to raw
        return this->m_iconSet.callWrongIcon;
    }
    const offset_t current = indexToOffset(index);
    if (target < current) return this->m_iconSet.callUpIcon;
    return this->m_iconSet.callDownIcon;
}

QString DisasmBaseModel::getFormatedOffset(offset_t offset) const
{
    const offset_t convertedOffset = convertAddr(offset);
    if (convertedOffset == INVALID_ADDR) {
        return "<invalid>";
    }
    return QString::number(convertedOffset, 16);
}

QString DisasmBaseModel::getNamedOffset(offset_t offset) const
{
    if ( getTracer()->hasName(offset, Executable::RAW)) {
        QString name = getTracer()->getFunctionName(offset, Executable::RAW);
        return getFormatedOffset(offset) + " : " + name;
    }
    return getFormatedOffset(offset);
}
