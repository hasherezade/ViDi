#pragma once
#include <QtGui>
#include <bearparser.h>
#include "OffsetedView.h"

class ReferenceTracker : public QAction
{
    Q_OBJECT

  public:
    ReferenceTracker(QObject *parent) : QAction("Filter references", parent)
    {
        this->setCheckable(true);
        connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered(bool)));
    }

signals:
    void triggered(QString someActionInfo);
    void filterReferences(bool enableFilter);

private slots:
    void onTriggered(bool enable)
    {
        emit filterReferences(enable);
    }

public slots:

    void onReferenceFilterSet(bool isEnabled)
    {
        setChecked(isEnabled);
    }
};

class ShowColAction : public QAction
{
    Q_OBJECT

  public:
    ShowColAction(QObject *parent)
        : QAction("Show column", parent), m_tableView(NULL)
    {
        this->setCheckable(true);
        connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered(bool)));
    }

    void init(QTableView *table, int colNum, QString colName)
    {
        this->m_tableView = table;
        this->m_colNum = colNum;
        this->m_colName = colName;
        this->setText("Show column "+ m_colName);

        this->setChecked(!m_tableView->isColumnHidden(m_colNum));
    }

signals:
    void triggered(QString someActionInfo);

private slots:
    void onTriggered(bool enable)
    {
        if (!m_tableView) return;

        if (enable) {
            m_tableView->showColumn(m_colNum);
        } else {
            m_tableView->hideColumn(m_colNum);
        }
    }

protected:
    QTableView *m_tableView;
    int m_colNum;
    QString m_colName;
};
//-----
class FollowableOffsetedView : public OffsetedView
{
    Q_OBJECT

signals:
    void targetClicked(offset_t offset, Executable::addr_type);
    void filterReferences(bool enableFilter);

public:
    enum ACTIONS
    {
        ACTION_FOLLOW = 0,
        COUNT_ACTIONS
    };

    FollowableOffsetedView(QWidget *parent, Executable::addr_type targetAddrType = Executable::RVA)
        : OffsetedView(parent, targetAddrType),
        m_ContextMenu(this),
        m_isMenuEnabled(true)
    {
        connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuEvent(QPoint)) );
        initContextMenu();
        enableMenu(true);
    }

void enableMenu(bool enable)
{
    if (enable) this->setContextMenuPolicy(Qt::CustomContextMenu);
    else this->setContextMenuPolicy(Qt::DefaultContextMenu);

    m_isMenuEnabled = enable;
}

QMenu& getMenu() { return m_ContextMenu; }

void enableAction(enum FollowableOffsetedView::ACTIONS id, bool state)
{
    if (id >= COUNT_ACTIONS) return;
    if (m_contextActions[id] == NULL) return;
    m_contextActions[id]->setEnabled(state);
}

protected slots:
    void customMenuEvent(QPoint p)
    {
        offset_t currentOffset = getSelectedOffset();
        if (currentOffset == INVALID_ADDR) return;

        m_contextActions[ACTION_FOLLOW]->setText("Follow "+ translateAddrTypeName(m_targetAddrType) +": "+ QString::number(currentOffset, 16));
        m_ContextMenu.exec(mapToGlobal(p));
    }

    void followSelectedOffset()
    {
        offset_t currentOffset = getSelectedOffset();
        if (currentOffset != INVALID_ADDR) {
            emit targetClicked(currentOffset, m_targetAddrType);
        }
    }

    void setFlterReferences(bool enable)
    {
        emit filterReferences(enable);
    }

protected:
    //TODO: move it to util
    static QString translateAddrTypeName(const Executable::addr_type addrType)
    {
        switch (addrType) {
            case Executable::RAW : return "Raw";
            case Executable::RVA : return "RVA";
            case Executable::VA : return "VA";
        }
        return "";
    }

    void initContextMenu();

    virtual offset_t getSelectedOffset()
    {
        if (this->selectedIndexes().size() == 0) return INVALID_ADDR;
        QModelIndex current = this->selectedIndexes().back();
        return getOffsetFromUserData(current);
    }

    bool m_isMenuEnabled;
    QMenu m_ContextMenu;
    QAction *m_contextActions[COUNT_ACTIONS];
};
