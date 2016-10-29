#pragma once
#include <QtGui>
#include <bearparser.h>

#include "ui_filteredframe.h"
#include "OffsetedModel.h"
#include "FollowableOffsetedView.h"

namespace Ui {
    class FilteredFrame;
}

class FilteredFrame : public QFrame
{
    Q_OBJECT

public:
    FilteredFrame(QWidget *parent = 0)
        : QFrame(parent),
        m_model(NULL), m_proxyModel(NULL)
    {
        m_ui.setupUi(this);

        QStringList list;
        list << "Offset" << "Name" << "Refs";
        initFilter(list);
    }

    virtual void init(OffsetedModel *model);
    void initFilter(QStringList &list);

    FollowableOffsetedView* getView() const { return this->m_ui.followableView; }

public slots:
    void filterByColumn(const int colNum, const QString &str);

protected slots:
    void filterChanged();

protected:
    virtual void setupModels(OffsetedModel *model);

    Ui::FilteredFrame m_ui;

    OffsetedModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};
