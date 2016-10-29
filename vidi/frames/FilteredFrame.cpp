#include "FilteredFrame.h"

void FilteredFrame::initFilter(QStringList &list)
{
    this->m_ui.filterCombo->clear();
    this->m_ui.filterCombo->addItems(list);
}

void FilteredFrame::setupModels(OffsetedModel *model)
{
    if (!model) return;
    m_model = model;
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model);
    m_ui.followableView->setModel(m_proxyModel);

    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setDynamicSortFilter(true);
    m_ui.followableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui.followableView->setSortingEnabled(true);

    connect(m_ui.filterCombo, SIGNAL(currentIndexChanged ( int ) ), this, SLOT( filterChanged() ));
    connect( m_ui.filterEdit, SIGNAL(textChanged (const QString&)), this, SLOT( filterChanged() ));
}

void FilteredFrame::init(OffsetedModel *model)
{
    setupModels(model);
    //alternative way (to clicking)
    connect(m_ui.followableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        m_ui.followableView, SLOT(onIndexChanged(QModelIndex, QModelIndex)));
}

//todo: move it to superclass
void FilteredFrame::filterByColumn(const int colNum, const QString &str)
{
    if (!m_proxyModel) {
        return;
    }
    m_proxyModel->setFilterKeyColumn(colNum);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    if (str.size() == 0) {
        m_proxyModel->setFilterRegExp(".*");
        return;
    }
    m_proxyModel->setFilterFixedString(str);
}

//todo: move it to superclass
void FilteredFrame::filterChanged()
{
    filterByColumn(m_ui.filterCombo->currentIndex(), m_ui.filterEdit->text() );
}
