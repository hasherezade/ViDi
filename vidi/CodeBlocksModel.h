#pragma once

#include <QAbstractTableModel>
#include <QMenu>
#include "OffsetedModel.h"

class CodeBlocksModel : public OffsetedModel
{
    Q_OBJECT

public slots:
    void modelChanged() { reset(); }

public:
    enum COLS
    {
        COL_START = 0,
        COL_END,
        COL_NAME,
        COL_FUNC,
        COL_FORK_YES,
        COL_FORK_NO,
        COL_REFERED,
        COUNT_COL
    };

    CodeBlocksModel(QObject *v_parent)
        : OffsetedModel(v_parent, Executable::RVA)
    {
        init();
    }
    virtual ~CodeBlocksModel() { }

    int columnCount(const QModelIndex &parent) const { return COUNT_COL; }
    int rowCount(const QModelIndex &parent) const { return countElements(); }
    
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &data, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    
    virtual offset_t rowToOffset(const int row) const;

    CodeBlock* indexToCodeBlock(const QModelIndex &index) const
    {
        if (index.isValid() == false) {
            return NULL;
        }
        int row = index.row();
        return  rowToCodeBlock(index.row());
    }

    CodeBlock* rowToCodeBlock(const int row) const
    {
        int entriesCount = countElements();
        if (entriesCount == 0 || row >= entriesCount) return NULL;
        
        offset_t thunk = getTracer()->blocksList().at(row);
        CodeBlock *block = getTracer()->blockAt(thunk);
        return block;
    }

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

protected:
    void init();
    size_t countElements() const { return ( getTracer() == NULL) ? 0 : getTracer()->blocksList().size(); }

    bool isBlockContainingRef(CodeBlock *block) const;
    bool isBlockContainingEP(CodeBlock *block) const;
    bool isBlockRefered(CodeBlock *block) const;

    QColor m_yesColor, m_noColor;
};
