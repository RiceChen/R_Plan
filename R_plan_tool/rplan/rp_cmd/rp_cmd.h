#ifndef RP_CMD_H
#define RP_CMD_H

#include <QWidget>
#include <QTextEdit>
#include <QLayout>
#include <QProcess>
#include <QShortcut>

class rp_cmd : public QWidget
{
    Q_OBJECT
public:
    explicit rp_cmd(QWidget *parent = 0);

private:
    void rp_cmdWindowUI();

private:
    QProcess *rp_cmdProcess;


    QTextEdit *rp_cmdTextEdit;

private slots:
    void rp_writeCmd();
    void rp_readCmd();
    void rp_finishedProcess();
    void rp_errorProcess();

signals:

};

#endif // RP_CMD_H
