#include "rp_mini_udp_ota.h"

#include <QGroupBox>
#include <QGridLayout>

rp_mini_udp_ota::rp_mini_udp_ota(QWidget *parent)
    : QWidget(parent)
{
    rp_udpOtaWindowUI();
}

rp_mini_udp_ota::~rp_mini_udp_ota()
{
    ;
}

void rp_mini_udp_ota::rp_udpOtaWindowUI()
{
    QLabel *rp_udpOtaLabel = new QLabel(tr("固件路径:"));
    rp_udpOtaLabel->setStyleSheet("color:white");
    rp_fwPathLineEdit = new QLineEdit();

    rp_fwSelectPushButton = new QPushButton(tr("浏览"));
    rp_fwSelectPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");

    QGroupBox *rp_scanResultGroupBox = new QGroupBox(tr("扫描结果"));
    rp_scanResultGroupBox->setStyleSheet("color:white");
    rp_scanResultTableView = new QTableView();
    rp_scanResultTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rp_scanResultTableView->setSortingEnabled(true);
    rp_scanResultTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    rp_scanResultTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    rp_scanResultTableView->setShowGrid(false);
    rp_scanResultTableView->setAlternatingRowColors(true);
    QGridLayout *rp_scanResultLayout = new QGridLayout;
    rp_scanResultLayout->addWidget(rp_scanResultTableView, 0, 0);
    rp_scanResultGroupBox->setLayout(rp_scanResultLayout);


    QGridLayout *rp_mainLayout = new QGridLayout(this);
    rp_mainLayout->addWidget(rp_udpOtaLabel, 0, 0);
    rp_mainLayout->addWidget(rp_fwPathLineEdit, 0, 1);
    rp_mainLayout->addWidget(rp_fwSelectPushButton, 0, 2);
    rp_mainLayout->addWidget(rp_scanResultGroupBox, 1, 0);

    setLayout(rp_mainLayout);
}

void rp_mini_udp_ota::rp_udpOtaFuncConfig()
{

}
