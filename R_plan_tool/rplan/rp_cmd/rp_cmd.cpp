#include "rp_cmd.h"

rp_cmd::rp_cmd(QWidget *parent)
    : QWidget(parent)
{
    rp_cmdWindowUI();
}

void rp_cmd::rp_cmdWindowUI()
{
    rp_cmdTextEdit = new QTextEdit(tr("待完成。。。"));
    QPalette rp_cmdPalette = rp_cmdTextEdit->palette();
    rp_cmdPalette.setColor(QPalette::Base, Qt::black);
    rp_cmdPalette.setColor(QPalette::Text, Qt::green);
    rp_cmdTextEdit->setPalette(rp_cmdPalette);

    QGridLayout *rp_cmdLayout = new QGridLayout;
    rp_cmdLayout->addWidget(rp_cmdTextEdit);

    setLayout(rp_cmdLayout);
}

void rp_cmd::rp_writeCmd()
{

}

void rp_cmd::rp_readCmd()
{

}

void rp_cmd::rp_finishedProcess()
{

}

void rp_cmd::rp_errorProcess()
{

}
