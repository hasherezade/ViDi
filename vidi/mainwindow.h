#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"

#include "OffsetDialog.h"

#include "core/Controller.h"
#include "InfoTableModel.h"
#include "CodeBlocksModel.h"

#include "lists_mv/models.h"
#include "disasm_mv/models.h"

#include "ReferenceSelectModel.h"
#include "frames/FilteredFrame.h"
#include "frames/CodeBlocksFrame.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void setAddrType(Executable::addr_type);
    void selectedReferences(offset_t, Executable::addr_type) const;

public:
    enum TAB_ID {
        TAB_FUNCTIONS,
        TAB_STRINGS,
        TAB_IMPORTS,
        TAB_TAGS,
        COUNT_TABS
    };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void makeMenu();

    /* events */
    void dragEnterEvent(QDragEnterEvent* ev) { ev->accept(); }
    void dropEvent(QDropEvent* ev);

private slots:
    void info();
    void openExe();
    void onLoadingFailed(QString fileName);
    void onExeListChanged();
    void onFileIndexChanged(const QModelIndex &current, const QModelIndex &previous);
    void onDisasmIndexChanged(const QModelIndex &current, const QModelIndex &previous);

    void refreshTabTitles();
    void setDisasmFont();
    void showOffsetDialog();
    void onGoToTarget(offset_t, Executable::addr_type);

    void setAddrToRaw() { emit setAddrType(Executable::RAW); }
    void setAddrToRVA() { emit setAddrType(Executable::RVA); }
    void setAddrToVA() { emit setAddrType(Executable::VA); }

    //follow with appending to the history
    void onTargetClicked(offset_t target);
    void onTargetClicked(offset_t target, Executable::addr_type);

    void onExeSelected(ExeHandler *hndl);

    /* Navigation */
    void navigUndo()
    {
        ExeHandler *hndl = this->m_controller.selectedExe();
        if (!hndl) return;

        offset_t offset = hndl->getNavigHistory().undo();
        if (offset == INVALID_ADDR) return;
        this->followOffset(offset);
    }

    void navigRedo()
    {
        ExeHandler *hndl = this->m_controller.selectedExe();
        if (!hndl) return;

        offset_t offset = hndl->getNavigHistory().redo();
        if (offset == INVALID_ADDR) return;
        this->followOffset(offset);
    }

private:
    void loadFile(QString fileName);
    void addReferenceTracker(QMenu &dMenu);
    void initDisasmUpView();
    void initDisasmView(DisasmView *view, DisasmBaseModel *model);
    void initFollowableOffsetedView(FollowableOffsetedView *view);

    void initFilteredFrame(FilteredFrame *frame, OffsetedModel *model);
    void initCodeBlocksFrame(CodeBlocksFrame *frame, CodeBlocksModel *model);
    void initColResizeMode(OffsetedView &view);
    bool followOffset(offset_t target);

    offset_t getOffsetFromUserData(QAbstractItemModel *model, const QModelIndex &current);

    /* GUI */

    Ui::MainWindow m_ui;
    Controller m_controller;

    ReferenceSelectModel m_referenceSelectModel;

    InfoTableModel *m_infoModel;
    CodeBlocksModel *m_codeBlocksModel;
    DisasmModel *m_disasmUpModel;
    DisasmCmpModel* m_disasmYesModel, *m_disasmNoModel;
    DisasmViewSettings *m_disasmViewSettings;

    FunctionsModel *m_functionsModel;
    CalledImportsModel *m_calledImportsModel;
    StringsModel* m_stringModel;
    TagsModel* m_tagsModel;

    QTabWidget *m_tabWidget;

    QMutex m_exeViewMutex;

    FilteredFrame m_functionsFrame, m_importsFrame, m_stringsFrame, m_tagsFrame;
    CodeBlocksFrame m_codBlocksFrame;
    QAction *m_undoAction, *m_redoAction;

    QMap<TAB_ID, int> m_idToTabIndex;
    OffsetDialog m_offsetDialog;
};

