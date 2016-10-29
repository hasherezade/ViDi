#include "CodeBlocksFrame.h"

void CodeBlocksFrame::setupModels(OffsetedModel *model)
{
    if (!model) return;

    m_model = model;
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model);
    m_ui.followableView->setModel(m_proxyModel);

    m_proxyModel->setDynamicSortFilter(true);
    m_ui.followableView->setSortingEnabled(true);
    m_ui.followableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui.followableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(m_ui.filterCombo, SIGNAL(currentIndexChanged ( int ) ), this, SLOT( filterChanged() ));
    connect( m_ui.filterEdit, SIGNAL(textChanged (const QString&)), this, SLOT( onTextEdited() ));
}

void CodeBlocksFrame::init(OffsetedModel *model)
{
    setupModels(model);
    this->m_ui.followableView->setSortingEnabled(true);
    this->m_ui.followableView->horizontalHeader()->setStretchLastSection(true);
    this->m_ui.followableView->sortByColumn(2, Qt::DescendingOrder);

    this->m_ui.followableView->setMouseTracking(false);
    //connect signals
    connect(m_ui.followableView, SIGNAL(offsetChanged(const offset_t, const Executable::addr_type)), 
       this, SLOT(emitSelectedTarget(const offset_t, const Executable::addr_type)) );
}

//todo: move it to superclass
void CodeBlocksFrame::filterByColumn(const int colNum, const QString &str)
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
//
void CodeBlocksFrame::emitSelectedTarget(const offset_t offset, const Executable::addr_type addrType)
{
    //TODO: convert addrType
    emit targetClicked(offset);
}

//todo: move it to superclass
void CodeBlocksFrame::filterChanged()
{
    filterByColumn(m_ui.filterCombo->currentIndex(), m_ui.filterEdit->text() );
}

