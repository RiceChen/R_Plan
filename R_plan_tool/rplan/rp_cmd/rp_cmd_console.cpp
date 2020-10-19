#include "rp_cmd_console.h"

#include <QScrollBar>
#include <QDebug>

rp_cmd_console::rp_cmd_console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    rp_cmdWindowUI();

    rp_cmdFuncConfig();

    setAttribute(Qt::WA_DeleteOnClose);
}

rp_cmd_console::~rp_cmd_console()
{
    rp_cmdQprocess->kill();
}

void rp_cmd_console::rp_cmdWindowUI()
{
    document()->setMaximumBlockCount(100);
    QPalette rp_cmdPalette = palette();
    rp_cmdPalette.setColor(QPalette::Base, Qt::black);
    rp_cmdPalette.setColor(QPalette::Text, Qt::green);

    setPalette(rp_cmdPalette);
}

void rp_cmd_console::rp_cmdFuncConfig()
{
    insertPlainText(QString(tr("\\    |   /\n"
                               "- R plan - \n"
                               "/    |   \\  windows cmd console\n")));

    rp_cmdQprocess = new QProcess(this);
    rp_cmdQprocess->setProcessChannelMode(QProcess::MergedChannels);

    connect(rp_cmdQprocess,SIGNAL(readyRead()),this,SLOT(rp_cmdQProcessRecv()));
    connect(rp_cmdQprocess,SIGNAL(readyReadStandardOutput()),this,SLOT(rp_cmdQProcessRecv()));
    connect(rp_cmdQprocess,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(rp_cmdQProcessRecvError()));
    connect(rp_cmdQprocess,SIGNAL(finished(int)),this,SLOT(rp_cmdQProcessRecvFinished()));

    rp_cmdQprocess->start("cmd.exe");
}

void rp_cmd_console::rp_cmdQProcessSend(QString cmd)
{
    QByteArray rp_sendData = cmd.toLatin1();

    char *rp_sendDataCh = rp_sendData.data();
    qint64 rp_sendDataLen = cmd.length();
    rp_cmdQprocess->write(rp_sendDataCh, rp_sendDataLen);
}

void rp_cmd_console::rp_cmdQProcessRecv()
{
    QByteArray rp_recvData = rp_cmdQprocess->readAll();

    QString rp_recvDataStr = QString::fromLocal8Bit(rp_recvData);

    rp_putCmdData(rp_recvDataStr);
}

void rp_cmd_console::rp_cmdQProcessRecvError()
{
    int rp_errorCode  = rp_cmdQprocess->exitCode();
    QString rp_errorInfo = rp_cmdQprocess->errorString();

    insertPlainText(QString("error coed:%1").arg(rp_errorCode));
    insertPlainText(rp_errorInfo);
}

void rp_cmd_console::rp_cmdQProcessRecvFinished()
{
    int flag = rp_cmdQprocess->exitCode();
    qDebug() << "Process Recv Finished:" << flag << endl;
}


void rp_cmd_console::rp_putCmdData(QString cmd)
{
    insertPlainText(cmd);

    QScrollBar *rp_consoleScrollBar = verticalScrollBar();
    rp_consoleScrollBar->setValue(rp_consoleScrollBar->maximum());
}

void rp_cmd_console::keyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_Backspace == e->key())
    {
        if(rp_cmdSendData.length())
        {
            QString rp_cmdConsoleContent = toPlainText();
            rp_cmdConsoleContent = rp_cmdConsoleContent.left((rp_cmdConsoleContent.length() - 1));
            rp_cmdSendData = rp_cmdSendData.left((rp_cmdSendData.length() - 1));
            setPlainText("");
            insertPlainText(rp_cmdConsoleContent);
        }
    }
    else if(Qt::Key_Enter == e->key() || Qt::Key_Return == e->key())
    {
        insertPlainText("\n");
        rp_cmdSendData.append("\r\n");
        rp_cmdQProcessSend(rp_cmdSendData);
        rp_cmdSendData.clear();
    }
    else if((Qt::ControlModifier & e->modifiers()) != 0 && Qt::Key_C == e->key())
    {
        rp_cmdSendData.append("\r\n");
        rp_cmdQProcessSend(rp_cmdSendData);
        rp_cmdSendData.clear();
    }
    else
    {
        QPlainTextEdit::keyPressEvent(e);
        rp_cmdSendData.append(e->text().toLocal8Bit());
    }
}

void rp_cmd_console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    setFocus();
}

void rp_cmd_console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
}

void rp_cmd_console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
}

