#pragma once
#include <QtWidgets>
#include <bearparser.h>

#include "disasm_mv/DisasmBaseModel.h"

class DisasmViewSettings : public QObject
{
    Q_OBJECT
public:
    DisasmViewSettings(QObject *parent)
        : QObject(parent)
    {
        initDefault();
    }

    virtual ~DisasmViewSettings() {}
    
    QFont& font() { return m_Font; }
    QPalette& palette()  { return m_Palette; }
    QString& styleSheet()  { return m_styleSheet; }

    void setFont(QFont &font)
    {
        m_Font = font;
        emit fontChanged();
    }

signals:
    void settingsChanged();
    void fontChanged();

protected:
    void initDefault();

    QFont m_Font;
    QPalette m_Palette;
    QString m_styleSheet;
};

class DisasmView : public QTableView
{
    Q_OBJECT

signals:
    void targetClicked(offset_t offset);

public:
    DisasmView(QWidget *parent)
        : QTableView(parent),
        m_disasmModel(NULL),
        m_ContextMenu(this), m_isMenuEnabled(false),
        m_disasmViewSettings(NULL)
    {
        connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuEvent(QPoint)) );
        enableMenu(false);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setMouseTracking(true);
    }

    void initStyle(DisasmViewSettings *viewSettings);
    
    void setModel(DisasmBaseModel *model)
    {
        this->m_disasmModel = model;
        QTableView::setModel(model);
    }

    void enableMenu(bool enable)
    {
        if (enable) this->setContextMenuPolicy(Qt::CustomContextMenu);
        else this->setContextMenuPolicy(Qt::DefaultContextMenu);

        m_isMenuEnabled = enable;
    }

    QMenu& getMenu() { return m_ContextMenu; }

    /* events */
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void leaveEvent(QEvent *ev);

public slots:
    void resetStyle();
    void resetFont();

protected slots:
    void customMenuEvent(QPoint p)
    {
        m_ContextMenu.exec(mapToGlobal(p)); 
    }

protected:

    offset_t getClickableOffset(const QModelIndex& index);
    bool m_isMenuEnabled;
    QMenu m_ContextMenu;

    DisasmBaseModel *m_disasmModel;
    DisasmViewSettings *m_disasmViewSettings;
};
