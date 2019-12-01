#pragma once
#include <QtGui>
#include <bearparser/core.h>

#include "CodeBlocksModel.h"
#include "OffsetedView.h"
#include "ui_codeblocksframe.h"

namespace Ui {
    class FilteredFrame;
}

class CodeBlocksFrame : public QFrame
{
    Q_OBJECT

signals:
    void targetClicked(offset_t offset);
    void referenceFilterSet(bool isEnabled);

public slots:
    void emitSelectedTarget(const offset_t offset, const Executable::addr_type aType);

    bool enableReferencesFilter(bool enable)
    {
        if (enable) {
            m_ui.filterCombo->setCurrentIndex(CodeBlocksModel::COL_REFERED);
            m_ui.filterEdit->setText(">");
            return true;
        }
        m_ui.filterEdit->setText("");
        return false;
    }

protected slots:
    void filterByColumn(const int colNum, const QString &str);
    void filterChanged();
    
    void onTextEdited()
    {
        filterChanged();
        //TODO: emit signal only if state changed
        emit referenceFilterSet(isFilterEnabled());
    }

public:
    CodeBlocksFrame(QWidget *parent = 0)
        : QFrame(parent)
    {
        m_ui.setupUi(this);

        QStringList blocksCol;
        blocksCol <<"Start" << "End" << "Name" << "Condition" << "YES" << "NO" <<"Refering";
        this->m_ui.filterCombo->addItems(blocksCol);
    }

    QTableView* getView() const { return this->m_ui.followableView; }

    void setupModels(OffsetedModel *model);
    virtual void init(OffsetedModel *model);

    //todo: move it to superclass
    bool isFilterEnabled()
    {
        filterByColumn(m_ui.filterCombo->currentIndex(), m_ui.filterEdit->text());
        if (m_ui.filterEdit->text().size() == 0) {
            return false;
        }
        return true;
    }

protected:

    Ui::CodeBlocksFrame m_ui;
    OffsetedModel *m_model;
    QSortFilterProxyModel *m_proxyModel;

};
