#include "rplan.h"

#include <QtWidgets>

rplan::rplan(QWidget *parent)
    : QMainWindow(parent)
{
    rp_mainWindowUI();

    rp_createStatusBar();

}

void rplan::rp_mainWindowUI()
{
    /* set window size */
    const QIcon rp_mainWindowIcon = QIcon::fromTheme("mainWindow", QIcon(":/image/logo.png"));
    setFixedSize(1000, 800);
    setWindowIcon(rp_mainWindowIcon);
    setWindowTitle(tr("r-plan"));

    /* File */
    QToolBar *rp_fileToolBar = addToolBar(tr("File"));
    // new file
    const QIcon rp_newFileIcon = QIcon::fromTheme("edit-newFile", QIcon(":/image/new.png"));
    QAction *rp_newFileAction = new QAction(rp_newFileIcon, tr("&NewFile"), this);
    rp_newFileAction->setShortcuts(QKeySequence::New);
    rp_newFileAction->setStatusTip(tr("Create a new file"));
//    connect(rp_newFileAction, &QAction::triggered, this, &rplan::newFile);
    rp_fileToolBar->addAction(rp_newFileAction);
    // open file
    const QIcon rp_openFileIcon = QIcon::fromTheme("edit-openFile", QIcon(":/image/open.png"));
    QAction *rp_openFileAction = new QAction(rp_openFileIcon, tr("&OpenFile"), this);
    rp_openFileAction->setShortcuts(QKeySequence::Open);
    rp_openFileAction->setStatusTip(tr("Open an existing file"));
    rp_fileToolBar->addAction(rp_openFileAction);
    //save file
    const QIcon rp_saveFileIcon = QIcon::fromTheme("edit-saveFile", QIcon(":/image/save.png"));
    QAction *rp_saveFileAction = new QAction(rp_saveFileIcon, tr("&SaveFile"), this);
    rp_saveFileAction->setShortcuts(QKeySequence::Save);
    rp_saveFileAction->setStatusTip(tr("Save the document to disk"));
    rp_fileToolBar->addAction(rp_saveFileAction);

    /* Edit */
    QToolBar *rp_editToolBar = addToolBar(tr("Edit"));
    // cut
    const QIcon rp_cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/image/cut.png"));
    QAction *rp_cutAction = new QAction(rp_cutIcon, tr("&Cut"), this);
    rp_cutAction->setShortcuts(QKeySequence::Cut);
    rp_cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    rp_editToolBar->addAction(rp_cutAction);
    //copy
    const QIcon rp_copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/image/copy.png"));
    QAction *rp_copyAction = new QAction(rp_copyIcon, tr("&Copy"), this);
    rp_copyAction->setShortcuts(QKeySequence::Copy);
    rp_copyAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    rp_editToolBar->addAction(rp_copyAction);
    //paste
    const QIcon rp_pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/image/paste.png"));
    QAction *rp_pasteAction = new QAction(rp_pasteIcon, tr("&Paste"), this);
    rp_pasteAction->setShortcuts(QKeySequence::Paste);
    rp_pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    rp_editToolBar->addAction(rp_pasteAction);

    /* Help */
    QToolBar *rp_helpToolBar = addToolBar(tr("Help"));
    const QIcon rp_helpIcon = QIcon::fromTheme("edit-help", QIcon(":/image/help.png"));
    QAction *rp_helpAction = new QAction(rp_helpIcon, tr("&help"), this);
    connect(rp_helpAction, &QAction::triggered, this, &rplan::rp_help);
    rp_helpAction->setStatusTip(tr("Help"));
    rp_helpToolBar->addAction(rp_helpAction);

    /* TabWidget */
    rp_funTabWidget = new QTabWidget;
    rp_cmdWidget = new rp_cmd_console();
    const QIcon rp_cmdIconTab = QIcon::fromTheme("tab-cmd", QIcon(":/image/cmd.png"));
    rp_funTabWidget->addTab(rp_cmdWidget, rp_cmdIconTab, tr("cmd console"));

    rp_rttOtaPackWidget = new rp_rtt_ota_pack();
    const QIcon rp_rttOtaPackIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/pack.png"));
    rp_funTabWidget->addTab(rp_rttOtaPackWidget, rp_rttOtaPackIconTab, tr("rtt ota pack"));

    rp_findFileWidget = new rp_find_file();
    const QIcon rp_findFileIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/find_file.png"));
    rp_funTabWidget->addTab(rp_findFileWidget, rp_findFileIconTab, tr("find file"));

    setCentralWidget(rp_funTabWidget);
}

void rplan::rp_createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


rplan::~rplan()
{
}

void rplan::rp_help()
{
    QMessageBox::about(this, tr("Help"),
                       tr("R plan tool v1.0.0      "));

}

