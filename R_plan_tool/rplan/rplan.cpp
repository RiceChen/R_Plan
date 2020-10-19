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

    const QIcon rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
    rp_ceShiftAction = new QAction(rp_ceShiftIcon, tr("&ceShift"), this);
    connect(rp_ceShiftAction, &QAction::triggered, this, &rplan::rp_ceShift);
    rp_ceShiftAction->setStatusTip(tr("Chinese-English shift"));
    rp_helpToolBar->addAction(rp_ceShiftAction);

    const QIcon rp_helpIcon = QIcon::fromTheme("edit-help", QIcon(":/image/help.png"));
    QAction *rp_helpAction = new QAction(rp_helpIcon, tr("&help"), this);
    connect(rp_helpAction, &QAction::triggered, this, &rplan::rp_help);
    rp_helpAction->setStatusTip(tr("Help"));
    rp_helpToolBar->addAction(rp_helpAction);

    /* TabWidget */
    rp_funTabWidget = new QTabWidget;
    rp_funTabWidget->setAttribute(Qt::WA_StyledBackground);
    rp_funTabWidget->setStyleSheet("QTabWidget{background-color:rgb(50, 50, 50);}\
                                   QTabWidget::pane{border: 1px solid rgb(100, 100, 100);}\
                                   QTabBar::tab{background:rgb(80, 80, 80);color:rgb(200, 200, 200);}\
                                   QTabBar::tab:hover{background:rgb(255, 255, 255, 100);color:black;}\
                                   QTabBar::tab:selected{border:1px solid rgb(100, 100, 100); background-color:rgb(50, 50, 50);color:rgb(255, 255, 255)}");

   // NetAid
   rp_netAidWidget = new rp_net_aid();
   const QIcon rp_netAidIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/net_aid.png"));
   rp_funTabWidget->addTab(rp_netAidWidget, rp_netAidIconTab, tr("NetAid"));

   // udp ota
   rp_udpOtaWidget = new rp_mini_udp_ota();
   const QIcon rp_udpOtaIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/ota.png"));
   rp_funTabWidget->addTab(rp_udpOtaWidget, rp_udpOtaIconTab, tr("udp ota"));

    // rtt ota pack
    rp_rttOtaPackWidget = new rp_rtt_ota_pack();
    const QIcon rp_rttOtaPackIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/pack.png"));
    rp_funTabWidget->addTab(rp_rttOtaPackWidget, rp_rttOtaPackIconTab, tr("rtt ota pack"));

    // find file
    rp_findFileWidget = new rp_find_file();
    const QIcon rp_findFileIconTab = QIcon::fromTheme("tab-rtt_ota_pack", QIcon(":/image/find_file.png"));
    rp_funTabWidget->addTab(rp_findFileWidget, rp_findFileIconTab, tr("find file"));

    //cmd
    rp_cmdWidget = new rp_cmd_console();
    const QIcon rp_cmdIconTab = QIcon::fromTheme("tab-cmd", QIcon(":/image/cmd.png"));
    rp_funTabWidget->addTab(rp_cmdWidget, rp_cmdIconTab, tr("cmd console"));

    connect(rp_funTabWidget, &QTabWidget::currentChanged, this, &rplan::rp_switchFuncTabWidget);

    setCentralWidget(rp_funTabWidget);

    QPalette rp_palette(this->palette());
    rp_palette.setColor(QPalette::Background, QColor (30, 30 , 30, 255));
    this->setPalette(rp_palette);
}

void rplan::rp_createStatusBar()
{
    statusBar()->setStyleSheet("color:white;");
    statusBar()->showMessage(tr("Ready"));
}

rplan::~rplan()
{
}

void rplan::rp_switchFuncTabWidget()
{
    int rp_funcTabIndex = rp_funTabWidget->currentIndex();
    QIcon rp_ceShiftIcon;
    switch(rp_funcTabIndex)
    {
    case rp_netAidTab:
    {
        if(rp_netAidWidget->rp_netAidGetLanguageType())
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        }
        else
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        }
        break;
    }
    case rp_udpOtaTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        break;
    }
    case rp_rttOtaPackTab:
    {
        if(rp_rttOtaPackWidget->rp_rttOtaPackGetlanguageType())
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        }
        else
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        }
        break;
    }
    case rp_findFileTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        break;
    }
    case rp_cmdTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);

        break;
    }
    default:
    {
        break;
    }
    }
}

void rplan::rp_ceShift()
{
    int rp_funcTabIndex = rp_funTabWidget->currentIndex();
    QIcon rp_ceShiftIcon;

    qDebug() << rp_funcTabIndex;
    switch(rp_funcTabIndex)
    {
    case rp_netAidTab:
    {
        if(rp_netAidWidget->rp_netAidGetLanguageType())
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
            rp_netAidWidget->rp_netAidLSetLanguageType(FALSE);
        }
        else
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
            rp_netAidWidget->rp_netAidLSetLanguageType(TRUE);
        }
        break;
    }
    case rp_udpOtaTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        break;
    }
    case rp_rttOtaPackTab:
    { 
        if(rp_rttOtaPackWidget->rp_rttOtaPackGetlanguageType())
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
            rp_rttOtaPackWidget->rp_rttOtaPackSetlanguageType(FALSE);
        }
        else
        {
            rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/ch.png"));
            rp_ceShiftAction->setIcon(rp_ceShiftIcon);
            rp_rttOtaPackWidget->rp_rttOtaPackSetlanguageType(TRUE);
        }
        break;
    }
    case rp_findFileTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);
        break;
    }
    case rp_cmdTab:
    {
        rp_ceShiftIcon = QIcon::fromTheme("edit-CH-EN-shift", QIcon(":/image/en.png"));
        rp_ceShiftAction->setIcon(rp_ceShiftIcon);

        break;
    }

    default:
    {
        break;
    }
    }
}

void rplan::rp_help()
{
    QMessageBox::about(this, tr("Help"),
                       tr("R plan tool v1.0.0"));

}

