#include "mainwindow.h"

#include <QtGui>

const QString APP_NAME = "ViDi";
const QString APP_EXTNAME = "Visual Disassembler";
const QString APP_VERSION = "0.2.5.3";
const QString SITE_LINK = "https://hshrzd.wordpress.com/";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_offsetDialog(this),
        m_codeBlocksModel(NULL), m_referenceSelectModel(this)
{
    m_ui.setupUi(this);
    this->setWindowTitle(APP_NAME + " "+ APP_EXTNAME + " " + APP_VERSION);
    setAcceptDrops(true);

    makeMenu();

    QLabel *urlLabel = new QLabel(m_ui.statusBar);
    m_ui.statusBar->addPermanentWidget(urlLabel);
    urlLabel->setTextFormat(Qt::RichText);
    urlLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    urlLabel->setOpenExternalLinks(true);
    urlLabel->setText("<a href=\""+ SITE_LINK +"\">"+SITE_LINK+"</a>");

    this->m_infoModel = new InfoTableModel(m_ui.fileView);
    m_infoModel->setExecutables(&m_controller.m_exes);
    m_ui.fileView->setModel(m_infoModel);

    m_disasmUpModel = new DisasmModel(m_ui.disasmView);
    m_disasmYesModel = new DisasmCmpModel(m_ui.yesView);
    m_disasmNoModel = new DisasmCmpModel(m_ui.noView);

    m_disasmViewSettings = new DisasmViewSettings(this);
    initDisasmView(m_ui.disasmView, m_disasmUpModel);
    initDisasmView(m_ui.yesView, m_disasmYesModel);
    initDisasmView(m_ui.noView,  m_disasmNoModel);

    m_disasmNoModel->setTwinModel(m_disasmYesModel);
    m_disasmYesModel->setTwinModel(m_disasmNoModel);

    m_ui.yesView->horizontalHeader()->setSectionResizeMode(DisasmCmpModel::COL_ICON, QHeaderView::ResizeToContents);
    m_ui.yesView->horizontalHeader()->setSectionResizeMode(DisasmCmpModel::COL_MATCH, QHeaderView::ResizeToContents);
    m_ui.noView->horizontalHeader()->setSectionResizeMode(DisasmCmpModel::COL_ICON, QHeaderView::ResizeToContents);
    m_ui.noView->horizontalHeader()->setSectionResizeMode(DisasmCmpModel::COL_MATCH, QHeaderView::ResizeToContents);

    connect(&m_controller.m_exes, SIGNAL(exeListChanged()), this, SLOT(onExeListChanged()));

    // connect references model
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), &m_referenceSelectModel, SLOT( setExecutable(ExeHandler*)) );
    // connect windows
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), this, SLOT( onExeSelected(ExeHandler*)) );

    connect(this, SIGNAL(selectedReferences(offset_t, Executable::addr_type)),
        &m_referenceSelectModel, SLOT(onSelectedReferences(offset_t, Executable::addr_type))
    );

    /* others : connect by selection model */
    connect(m_ui.disasmView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        this, SLOT(onDisasmIndexChanged(QModelIndex, QModelIndex)));

    connect(m_ui.fileView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        this, SLOT(onFileIndexChanged(QModelIndex, QModelIndex)));

    m_tabWidget = new QTabWidget(this);
    this->m_ui.dockWidget->setWidget(this->m_ui.filesFrame);
    m_ui.fileView->setDropIndicatorShown(false);
    m_ui.fileView->setRootIsDecorated(false);

    this->m_ui.dockWidget->setWindowTitle("Loaded files");
    this->m_ui.dockWidget_2->setWindowTitle("References");
    this->m_ui.dockWidget_2->setWidget(m_tabWidget);

    m_idToTabIndex[MainWindow::TAB_FUNCTIONS] = m_tabWidget->addTab(&m_functionsFrame,"Local functions");
    m_idToTabIndex[MainWindow::TAB_IMPORTS] = m_tabWidget->addTab(&m_importsFrame,"Imports");
    m_idToTabIndex[MainWindow::TAB_STRINGS] = m_tabWidget->addTab(&m_stringsFrame,"Strings");
    m_idToTabIndex[MainWindow::TAB_TAGS] = m_tabWidget->addTab(&m_tagsFrame,"Tags");

    connect(&m_codBlocksFrame, SIGNAL(targetClicked(offset_t)), this, SLOT(onTargetClicked(offset_t)));
    this->m_ui.upperSplitter->insertWidget(0, &m_codBlocksFrame);
    m_functionsModel = new FunctionsModel(this);
    m_calledImportsModel = new CalledImportsModel(this);
    m_stringModel = new StringsModel(this);
    m_tagsModel = new TagsModel(this);
    m_codeBlocksModel = new CodeBlocksModel(this);

    connect(m_functionsModel, SIGNAL(resetView()), this, SLOT(refreshTabTitles()) );
    connect(m_calledImportsModel, SIGNAL(resetView()), this, SLOT(refreshTabTitles()) );
    connect(m_tagsModel, SIGNAL(resetView()), this, SLOT(refreshTabTitles()) );
    connect(m_stringModel, SIGNAL(resetView()), this, SLOT(refreshTabTitles()) );

    initFilteredFrame(&m_functionsFrame, this->m_functionsModel);
    initFilteredFrame(&m_importsFrame, this->m_calledImportsModel);
    initFilteredFrame(&m_stringsFrame, this->m_stringModel);
    initFilteredFrame(&m_tagsFrame, this->m_tagsModel);
    initCodeBlocksFrame(&m_codBlocksFrame, m_codeBlocksModel);

    m_importsFrame.getView()->enableAction(FollowableOffsetedView::ACTION_FOLLOW, false);
    m_stringsFrame.getView()->enableAction(FollowableOffsetedView::ACTION_FOLLOW, false);

    QStringList stringsCol;
    stringsCol <<"Offset" << "String" << "Refs";
    this->m_stringsFrame.initFilter(stringsCol);

    initDisasmUpView();
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), &m_offsetDialog, SLOT( setExecutable(ExeHandler*) ) );
    connect (&m_offsetDialog, SIGNAL(goToTarget(offset_t, Executable::addr_type)), this, SLOT(onGoToTarget(offset_t, Executable::addr_type)) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::refreshTabTitles()
{
    m_tabWidget->setTabText(m_idToTabIndex[MainWindow::TAB_FUNCTIONS], "Local f. ("+QString::number(this->m_functionsModel->countElements()) +")");
    m_tabWidget->setTabText(m_idToTabIndex[MainWindow::TAB_IMPORTS], "Imports ("+QString::number(this->m_calledImportsModel->countElements()) +")");
    m_tabWidget->setTabText(m_idToTabIndex[MainWindow::TAB_STRINGS], "Strings ("+QString::number(this->m_stringModel->countElements()) +")");
    m_tabWidget->setTabText(m_idToTabIndex[MainWindow::TAB_TAGS], "Tags ("+QString::number(this->m_tagsModel->countElements()) +")");
}

void MainWindow::initDisasmView(DisasmView *view, DisasmBaseModel *model)
{
    view->setModel(model);
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), model, SLOT( setExecutable(ExeHandler*) ) );
    connect(view, SIGNAL(targetClicked(offset_t)), this, SLOT(onTargetClicked(offset_t)));

    model->setReferenceSelectionModel(&m_referenceSelectModel);
    view->initStyle(m_disasmViewSettings);
}

void MainWindow::initDisasmUpView()
{
    m_ui.disasmView->enableMenu(true);
    m_ui.disasmView->horizontalHeader()->setSectionResizeMode(DisasmModel::COL_REFS, QHeaderView::ResizeToContents);
    m_ui.disasmView->horizontalHeader()->setSectionResizeMode(DisasmModel::COL_ICON, QHeaderView::ResizeToContents);
    m_ui.disasmView->horizontalHeader()->setSectionResizeMode(DisasmModel::COL_CODE, QHeaderView::Stretch);
    m_ui.disasmView->horizontalHeader()->setSectionResizeMode(DisasmModel::COL_HEX, QHeaderView::Stretch);

    QMenu &dMenu = m_ui.disasmView->getMenu();
    addReferenceTracker(dMenu);

    ShowColAction *showCol = new ShowColAction(this);
    showCol->init(m_ui.disasmView, DisasmModel::COL_HEX, "Hex");
    dMenu.addAction(showCol);

    dMenu.addSeparator();
    dMenu.addAction(m_undoAction);
    dMenu.addAction(m_redoAction);
}

void MainWindow::addReferenceTracker(QMenu &dMenu)
{
    ReferenceTracker *refAction = new ReferenceTracker(&dMenu);
    dMenu.addAction(refAction);
    connect(refAction, SIGNAL(filterReferences(bool)), &this->m_codBlocksFrame, SLOT(enableReferencesFilter(bool)) );
    connect(&this->m_codBlocksFrame, SIGNAL(referenceFilterSet(bool)), refAction, SLOT(onReferenceFilterSet(bool)) );
}

void MainWindow::initFollowableOffsetedView(FollowableOffsetedView *view)
{
    addReferenceTracker(view->getMenu());
    connect(view, SIGNAL(targetClicked(offset_t, Executable::addr_type)), this, SLOT(onTargetClicked(offset_t, Executable::addr_type)));

    connect(view, SIGNAL(offsetChanged(const offset_t, const Executable::addr_type)),
        &m_referenceSelectModel, SLOT(onSelectedReferences(const offset_t, const Executable::addr_type)) );
}

void MainWindow::initFilteredFrame(FilteredFrame *frame, OffsetedModel *model)
{
    if (!frame || !model) return;

    FollowableOffsetedView* view = frame->getView();
    if (!view) return;

    frame->init(model);
    model->setReferenceSelectionModel(&m_referenceSelectModel);
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), model, SLOT( setExecutable(ExeHandler*) ) );
    connect(this, SIGNAL(setAddrType(Executable::addr_type)), model, SLOT( onSetAddrType(Executable::addr_type)) );
    initFollowableOffsetedView(view);

    initColResizeMode(*view);
}

void MainWindow::initCodeBlocksFrame(CodeBlocksFrame *frame, CodeBlocksModel *model)
{
    if (!frame || !model) return;

    QTableView* view = frame->getView();
    if (!view) return;

    frame->init(model);
    model->setReferenceSelectionModel(&m_referenceSelectModel);
    connect(&m_controller, SIGNAL( exeSelected(ExeHandler*)), model, SLOT( setExecutable(ExeHandler*) ) );
    connect(this, SIGNAL(setAddrType(Executable::addr_type)), model, SLOT( onSetAddrType(Executable::addr_type)) );
}


void MainWindow::initColResizeMode(OffsetedView &view)
{
    view.horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view.horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    view.horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

void MainWindow::dropEvent(QDropEvent* ev)
{
    QList<QUrl> urls = ev->mimeData()->urls();
    QList<QUrl>::Iterator urlItr;
    QCursor cur = this->cursor();

    for (urlItr = urls.begin() ; urlItr != urls.end(); urlItr++) {
        loadFile(urlItr->toLocalFile());
    }
}

void MainWindow::openExe()
{
    QString fileName = QFileDialog::getOpenFileName(
        NULL, "Open executable",
        QDir::homePath(),
        "Exe Files (*.exe);;DLL Files (*.dll);;All files (*)"
    );

    loadFile(fileName);
}

void MainWindow::loadFile(QString fileName)
{
    if (fileName.length() == 0) return;

    LoadProgressBar *progressBar = new LoadProgressBar(this);
    progressBar->setToolTip(fileName);
    progressBar->setStatusTip(fileName);
    this->m_ui.filesLayout->addWidget(progressBar);
    if (!m_controller.parse(fileName, progressBar)) {
        delete progressBar; // cannot start parsing
    }
}

void MainWindow::setDisasmFont()
{
    QFont font = QFontDialog::getFont(0, this->m_disasmViewSettings->font());
    this->m_disasmViewSettings->setFont(font);
}

void MainWindow::showOffsetDialog()
{
    if (m_controller.selectedExe() == NULL) {
        QMessageBox::warning(NULL,"Cannot go!", "No Exe selected");
        return;
    }
    m_offsetDialog.show();
}

void MainWindow::onGoToTarget(offset_t offset, Executable::addr_type aType)
{
    return onTargetClicked(offset, aType);
}

void MainWindow::onFileIndexChanged(const QModelIndex &current, const QModelIndex &previous)
{
    int elNum = m_infoModel->data(current, Qt::UserRole).toInt();
    ExeHandler *exeHndl = m_controller.m_exes.at(elNum);
    m_controller.selectExe(exeHndl);
}

void MainWindow::onExeSelected(ExeHandler *hndl)
{
    if (!hndl || !hndl->getExe()) return;
    const QString name = APP_NAME + " " + APP_VERSION + " [" + hndl->getFileName() + "]";
    setWindowTitle(name);
    refreshTabTitles();
    onTargetClicked(hndl->getExe()->getEntryPoint(Executable::RAW));
}

//follow with appending to the history
void MainWindow::onTargetClicked(offset_t target)
{
    ExeHandler *hndl = this->m_controller.selectedExe();
    if (!hndl) return;

    if (followOffset(target)) {
        //only clicking on target should append it to the navig history!
        hndl->getNavigHistory().append(target);
    }
}

void MainWindow::onTargetClicked(offset_t target, Executable::addr_type inType)
{
    ExeHandler *hndl = this->m_controller.selectedExe();
    if (!hndl || !hndl->getExe()) return;

    offset_t raw = hndl->getExe()->convertAddr(target, inType, Executable::RAW);
    if (raw == INVALID_ADDR) return;

    onTargetClicked(raw);
}

bool MainWindow::followOffset(offset_t target)
{
    if (target == INVALID_ADDR) return false;

    ExeHandler *hndl = this->m_controller.selectedExe();
    if (!hndl) return false;

    Tracer *dis = hndl->getTracer();
    if (!dis) return false;

    CodeBlock* block = m_controller.getBlockAtOffset(target);
    if (!block) {
        m_disasmUpModel->setCodeBlock(NULL);
        m_disasmYesModel->setCodeBlock(NULL);
        m_disasmNoModel->setCodeBlock(NULL);
        QMessageBox::warning(this,"Error","Cannot resolve this address: " + QString::number(target, 16));
        return false;
    }
    //success:
    m_disasmUpModel->setCodeBlock(block, target);
    ForkPoint *fork = dis->forkAt(block->getEndOffset());
    if (!fork) {
        m_disasmYesModel->setCodeBlock(NULL);
        m_disasmNoModel->setCodeBlock(NULL);
        return true;
    }
    CodeBlock* yesBlock = m_controller.getBlockAtOffset(fork->yesOffset);
    CodeBlock* noBlock = m_controller.getBlockAtOffset(fork->noOffset);
    m_disasmYesModel->setCodeBlock(yesBlock, fork->yesOffset);
    m_disasmNoModel->setCodeBlock(noBlock, fork->noOffset);
    return true;
}

offset_t MainWindow::getOffsetFromUserData(QAbstractItemModel *model, const QModelIndex &current)
{
    bool isOk;
    qint64 userData = model->data(current, Qt::UserRole).toULongLong(&isOk);
    return isOk ? userData : INVALID_ADDR;
}

void MainWindow::onDisasmIndexChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid()) return;
    if (current.column() == DisasmModel::COL_REFS) {
         emit selectedReferences(this->getOffsetFromUserData(m_disasmUpModel, current), Executable::RAW);
    }
}

void MainWindow::makeMenu()
{
    QMenu *menu = m_ui.menuFile;
    /* File */
    QAction *openAction = new QAction("Open executable", menu);
    openAction->setIcon(QIcon(":/icons/Add.ico"));
    openAction->setShortcuts(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openExe()));
    menu->addAction(openAction);
    menu->addSeparator();

    QAction *openTagsAction = new QAction("Import tags", menu);
    connect(openTagsAction, SIGNAL(triggered()), &m_controller, SLOT(openTags()));
    menu->addAction(openTagsAction);

    QAction *saveTagsAction = new QAction("Export tags", menu);
    connect(saveTagsAction, SIGNAL(triggered()), &m_controller, SLOT(saveTags()));
    menu->addAction(saveTagsAction);

    /* Settings */
    menu = m_ui.menuSettings;
    QMenu *addrTypeMenu = new QMenu("Address type", menu);
    menu->addMenu(addrTypeMenu);

    QAction *rawAction = new QAction("Set raw (file offsets)", addrTypeMenu);
    QAction *rvaAction = new QAction("Set RVA", addrTypeMenu);
    QAction *vaAction = new QAction("Set VA", addrTypeMenu);
    addrTypeMenu->addAction(rawAction);
    addrTypeMenu->addAction(rvaAction);
    addrTypeMenu->addAction(vaAction);

    rawAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    rvaAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    vaAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

    connect(rawAction, SIGNAL(triggered()), this , SLOT(setAddrToRaw()) );
    connect(rvaAction, SIGNAL(triggered()), this , SLOT(setAddrToRVA()) );
    connect(vaAction, SIGNAL(triggered()), this , SLOT(setAddrToVA()) );

    QAction *fontAction = new QAction("Change disasm font",menu);
    connect(fontAction, SIGNAL(triggered()), this, SLOT(setDisasmFont()) );
    menu->addAction(fontAction);

    /* Navigation */
    menu = m_ui.menuNavigation;
    QAction* goToRvaAction = new QAction("Go to offset", menu);
    connect(goToRvaAction, SIGNAL(triggered()), this, SLOT(showOffsetDialog()) );
    menu->addAction(goToRvaAction);
    goToRvaAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

    m_undoAction = new QAction("Undo", menu);
    m_undoAction->setShortcut(QKeySequence(Qt::Key_Minus));
    connect(m_undoAction, SIGNAL(triggered()), this , SLOT(navigUndo()));
    menu->addAction(m_undoAction);

    m_redoAction = new QAction("Redo", menu);
    m_redoAction->setShortcut(QKeySequence(Qt::Key_Plus));
    connect(m_redoAction, SIGNAL(triggered()), this , SLOT(navigRedo()));
    menu->addAction(m_redoAction);

    /* Info */
    menu = m_ui.menuAbout;
    QAction *aboutAction = new QAction("About", menu);
    aboutAction->setIcon(QIcon(":/favicon.ico"));
    connect(aboutAction, SIGNAL(triggered()), this , SLOT(info()));
    menu->addAction(aboutAction);
}

void MainWindow::onLoadingFailed(QString fileName)
{
    QMessageBox::warning(NULL,"Error!", "Cannot load the file:" + fileName);
}

void MainWindow::onExeListChanged()
{
    m_ui.statusBar->showMessage("Loaded");
}

void MainWindow::info()
{
    int ret = 0;
    int count = 0;
    QPixmap p(":/favicon.ico");
    QString appName = APP_NAME + " "+ APP_EXTNAME + "<br/>version: " + APP_VERSION + " Qt5";
    QString msg = "<b>" + appName + "</b><br/>";
    msg += "site: <a href=\""+ SITE_LINK +"\">"+SITE_LINK+"</a><br/>";
    msg += "author: hasherezade<br/><br/>";

#ifdef BUILD_WITH_UDIS86
    msg += "disassembly powered by: udis86";
#else
    msg += "disassembly powered by: <a href=\"http://www.capstone-engine.org/\">capstone engine</a><br/><br/>";
#endif

    msg += "THIS TOOL IS PROVIDED \"AS IS\" WITHOUT WARRANTIES OF ANY KIND. <br/>\
        Use it at your own risk and responsibility.<br/>\
        Only for research purpose. Do not use it to break the law!";

    QMessageBox msgBox;
    msgBox.setWindowTitle("Info");

    msgBox.setText(msg);
    msgBox.setAutoFillBackground(true);
    msgBox.setIconPixmap(p);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}
