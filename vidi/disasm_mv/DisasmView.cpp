#include "DisasmView.h"

void DisasmViewSettings::initDefault()
{
    m_Font = QFont("TypeWriter");
    m_Font.setStyleHint(QFont::TypeWriter);
    m_Font.setPointSize(10);
    m_Font.setLetterSpacing(QFont::AbsoluteSpacing, 0);
    m_Font.setBold(true);
    m_Font.setStretch(QFont::Unstretched);

    /* init palette */
    m_Palette.setColor(QPalette::Highlight, QColor("lavender"));
    m_Palette.setColor(QPalette::HighlightedText, QColor("AliceBlue"));

    m_Palette.setColor(QPalette::Base, QColor("black")); 
    m_Palette.setColor(QPalette::AlternateBase, QColor("darkblue"));
    m_Palette.setColor(QPalette::Text, QColor("lime"));

    /* init style sheet */
    m_styleSheet = "selection-background-color: darkblue;";
}

//---

void DisasmView::initStyle(DisasmViewSettings *viewSettings)
{
    if (m_disasmViewSettings == viewSettings) return;

    if (m_disasmViewSettings  != NULL) {
        disconnect(m_disasmViewSettings, SIGNAL(settingsChanged()), this, SLOT(resetStyle()));
    }
    m_disasmViewSettings = viewSettings;
    if (m_disasmViewSettings  == NULL) return;

    connect(m_disasmViewSettings, SIGNAL(fontChanged()), this, SLOT(resetFont()));
    connect(m_disasmViewSettings, SIGNAL(settingsChanged()), this, SLOT(resetStyle()));
    resetStyle();
}

void DisasmView::resetFont()
{
    const QFont &f = m_disasmViewSettings->font();
    QHeaderView *verticalHeader = this->verticalHeader();
    if (verticalHeader) {
        verticalHeader->setSectionResizeMode(QHeaderView::Interactive);
        verticalHeader->setDefaultSectionSize(static_cast<int>(f.pointSize()*1.8));
    }
    setFont(f);
}

void DisasmView::resetStyle()
{
    setPalette(m_disasmViewSettings->palette());
    setStyleSheet(m_disasmViewSettings->styleSheet());
    resetFont();
}

offset_t DisasmView::getClickableOffset(const QModelIndex& index)
{
    if (!m_disasmModel || !index.isValid()) {
        return INVALID_ADDR;
    }
    if (m_disasmModel->isClickable(index)) {
        return m_disasmModel->getOffsetToFollow(index);
    }
    return INVALID_ADDR;
}

void DisasmView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    event->accept();
    if (!this->m_disasmModel) {
        QTableView::mouseMoveEvent(event);
        return;
    }

    IndexHooverModel *hM = m_disasmModel->getHooverModel();

    if (m_disasmModel->isClickable(index)) {
        if (hM) hM->hooverIndex(index);
        setCursor(Qt::PointingHandCursor);
        return QTableView::mouseMoveEvent(event);
    }
    if (hM) hM->unhooverIndex();
    setCursor(Qt::ArrowCursor);
    return QTableView::mouseMoveEvent(event);
}

void DisasmView::leaveEvent(QEvent *ev)
{
    ev->accept();
    if (this->m_disasmModel) {
        IndexHooverModel *hM = m_disasmModel->getHooverModel();
        if (hM) hM->unhooverIndex();
    }
    setCursor(Qt::ArrowCursor);
    QTableView::leaveEvent(ev);
}

    //void mousePressEvent(QMouseEvent *event);

void DisasmView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    event->accept();
    if (!this->m_disasmModel) {
        QTableView::mouseDoubleClickEvent(event);
        return;
    }
    offset_t offset = getClickableOffset(index);
    if (offset != INVALID_ADDR) {
        //emit targetClicked(offset);
        QTableView::mouseDoubleClickEvent(event);
        emit targetClicked(offset);
        return;
    }
    QTableView::mouseDoubleClickEvent(event);
    return;
}
