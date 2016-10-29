#include "CodeBlocksModel.h"

#include <QFileDialog>
#include <QMessageBox>

#define EMPTY_NAME ""
#define EP_NAME "_start"
#define PREFIX_CONTAINS "@ "
#define PREFIX_REFERS "> "


void CodeBlocksModel::init()
{
    const int alphaVal = 100;
    m_yesColor =  QColor("lime");
    m_yesColor.setAlpha(alphaVal);
    m_noColor = QColor("red");
    m_noColor.setAlpha(alphaVal);
}

QVariant CodeBlocksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case COL_NAME : return "Name";
        case COL_START : return "Start " + this->addrTypeName();
        case COL_END : return "End " + this->addrTypeName();
        case COL_FUNC : return "Condition";
        case COL_FORK_YES : return "YES";
        case COL_FORK_NO : return "NO";
        case COL_REFERED: return "Refering";
    }
    return QVariant();
}

QVariant CodeBlocksModel::data(const QModelIndex &index, int role) const
{
    if (this->countElements() == 0 || getTracer() == NULL) return QVariant();


    CodeBlock* block = indexToCodeBlock(index);
    if (!block) return QVariant();

    offset_t refOffset = INVALID_ADDR;
    Executable::addr_type refsType = Executable::NOT_ADDR;
    if (this->m_refSelectModel) {
        refOffset = this->m_refSelectModel->getRefOffset();
        refsType = this->m_refSelectModel->getRefsType();
    }

    offset_t end = block->getEndOffset();
    ForkPoint *fork = getTracer()->forkAt(block->getEndOffset());
    const Executable::addr_type aType = Executable::RAW;
    int attribute = index.column();

    if (role == Qt::UserRole) {
        return qint64(block->start);
    }
    if (role == Qt::BackgroundColorRole) {
        if (fork && fork->noOffset != INVALID_ADDR) {
            switch (attribute) {
                case COL_FORK_YES: return m_yesColor;
                case COL_FORK_NO: return m_noColor;
            }
        }
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    switch (attribute) {
        case COL_NAME :
        {
            bool isContainingRef = isBlockContainingRef(block);
            QString prefix = isContainingRef ? PREFIX_CONTAINS : "";
            if (isBlockContainingEP(block))
                return prefix + EP_NAME;

            if ( getTracer()->hasName(block->start, Executable::RAW)) {
                //if have own name - get own name, with a prefix (converted to selected type)
                return prefix + getTracer()->getFunctionName(this->convertAddr(block->start, Executable::RAW), this->m_addrType);
            }
            if (! isContainingRef) return EMPTY_NAME;
            //get a name of ref, rather than block name (converted to selected type)
            return prefix + getTracer()->getFunctionName(this->convertAddr(refOffset, refsType), this->m_addrType);

        }
        case COL_START : return QString::number(this->convertAddr(block->start, aType), 16);
        case COL_END : return QString::number(this->convertAddr(end, aType), 16);
        case COL_FUNC : return getTracer()->getDisasmString(end, aType);
        case COL_FORK_YES:
        case COL_FORK_NO:
            {
                if (!fork) return "";
                if (attribute == COL_FORK_YES)
                    return QString::number(this->convertAddr(fork->yesOffset, aType), 16);
                if (attribute == COL_FORK_NO && fork->noOffset != INVALID_ADDR)
                    return QString::number(this->convertAddr(fork->noOffset, aType), 16);
            }
            break;
        case COL_REFERED :
            {
                if (!isBlockRefered(block)) return QVariant();
                const QString &name = getNameOfRefered();
                if (name.length() > 0) {
                    return PREFIX_REFERS + name;
                }
                return PREFIX_REFERS + getTracer()->getFunctionName(this->convertAddr(refOffset, refsType), this->m_addrType);
            }
    }
    return QVariant();
}

offset_t CodeBlocksModel::rowToOffset(const int row) const
{
    CodeBlock *block = rowToCodeBlock(row);
    if (!block) return INVALID_ADDR;
    return block->start;
}

Qt::ItemFlags CodeBlocksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    int section = index.column();

    if (section == COL_NAME) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool CodeBlocksModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    int section = index.column();
    int row = index.row();

    if (section == COL_NAME) {
        QString name = data.toString();
        if (name == EMPTY_NAME && name.length() > 0) return false;

        offset_t off = rowToOffset(row);
        m_ExeHandler->setFunctionName(off, Executable::RAW, name);
        reset();
        return true;
    }
    return false;
}

/* private */

bool CodeBlocksModel::isBlockContainingEP(CodeBlock *block) const
{
    if (!m_ExeHandler) return false;
    return (block->contains(m_ExeHandler->getExe()->getEntryPoint(Executable::RAW)));
}

bool CodeBlocksModel::isBlockContainingRef(CodeBlock *block) const
{
    if (this->m_refSelectModel == NULL) return false;
    return m_refSelectModel->isBlockContainingRef(block);
}

bool CodeBlocksModel::isBlockRefered(CodeBlock *block) const
{
    if (this->m_refSelectModel == NULL) return false;
    return m_refSelectModel->isBlockRefered(block);
}
