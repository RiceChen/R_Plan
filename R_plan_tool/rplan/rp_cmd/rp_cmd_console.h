#ifndef RP_CMD_CONSOLE_H
#define RP_CMD_CONSOLE_H

#include <QPlainTextEdit>
#include <QProcess>

class rp_cmd_console : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit rp_cmd_console(QWidget *parent = nullptr);
    ~rp_cmd_console();

private:
    void rp_cmdWindowUI();
    void rp_cmdFuncConfig();

public:
    void rp_putCmdData(QString cmd);

    void rp_cmdQProcessSend(QString cmd);


protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
    QProcess *rp_cmdQprocess;

    QString rp_cmdSendData;

public slots:
    void rp_cmdQProcessRecv();
    void rp_cmdQProcessRecvError();
    void rp_cmdQProcessRecvFinished();

signals:
    void rp_getCmdData(const QByteArray &data);
};

#endif // RP_CMD_CONSOLE_H
