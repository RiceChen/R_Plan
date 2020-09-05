#include "rp_net_aid.h"

#include <QHostInfo>
#include <QNetworkInterface>

rp_net_aid::rp_net_aid(QWidget *parent)
    : QWidget(parent)
{
    rp_netAidUI();

    rp_netAidFuncConfig();
}

void rp_net_aid::rp_netAidUI()
{
    QString rp_fontColor = "color:white";
    /* net set */
    rp_netSetGroupBox = new QGroupBox(tr("网络设置"));
    QGridLayout *rp_netSetLayout = new QGridLayout;
    rp_netSetGroupBox->setStyleSheet("color:white");

    rp_protocolTypeLabel = new QLabel(tr("(1) 协议类型"));
    rp_protocolTypeComboBox = new QComboBox;
    QStringList rp_protocolTypeStrList;
    rp_protocolTypeStrList << tr("UDP") << tr("TCP Client") << tr("TCP Server");
    rp_protocolTypeComboBox->addItems(rp_protocolTypeStrList);
    rp_protocolTypeComboBox->setCurrentIndex(2);
    rp_protocolTypeLabel->setStyleSheet("color:white");
    rp_protocolTypeComboBox->setStyleSheet("color:block");

    rp_localAddrLabel = new QLabel(tr("(2) 本地主机地址"));
    rp_localAddrComboBox = new QComboBox;
    rp_localAddrComboBox->setCurrentIndex(0);
    rp_localAddrLabel->setStyleSheet("color:white");
    rp_localAddrComboBox->setStyleSheet("color:block");

    rp_localPortLabel = new QLabel(tr("(3) 本地主机端口"));
    rp_localPortLineEdit = new QLineEdit(tr("1234"));
    rp_localPortLabel->setStyleSheet("color:white");
    rp_localPortLineEdit->setStyleSheet("color:block");

    rp_switchNetPushButton = new QPushButton(tr("打开"));
    rp_switchNetPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");

    rp_netSetLayout->addWidget(rp_protocolTypeLabel, 0, 0);
    rp_netSetLayout->addWidget(rp_protocolTypeComboBox, 1, 0);
    rp_netSetLayout->addWidget(rp_localAddrLabel, 2, 0);
    rp_netSetLayout->addWidget(rp_localAddrComboBox, 3, 0);
    rp_netSetLayout->addWidget(rp_localPortLabel, 4, 0);
    rp_netSetLayout->addWidget(rp_localPortLineEdit, 5, 0);
    rp_netSetLayout->addWidget(rp_switchNetPushButton, 6, 0);
    rp_netSetGroupBox->setLayout(rp_netSetLayout);

    /* net recv set*/
    rp_netRecvSetGroupBox = new QGroupBox(tr("接收设置"));
    rp_netRecvSetGroupBox->setStyleSheet("color:white");
    QGridLayout *rp_netRecvSetLayout = new QGridLayout;

    rp_recvTypeButtonGroup = new QButtonGroup(this);
    rp_recvTypeAsciiRadioButton = new QRadioButton(tr("ASCII"));
    rp_recvTypeAsciiRadioButton->setStyleSheet("color:white");
    rp_recvTypeHexRadioButton = new QRadioButton(tr("HEX"));
    rp_recvTypeHexRadioButton->setStyleSheet("color:white");
    rp_recvTypeButtonGroup->addButton(rp_recvTypeAsciiRadioButton, rp_asciiType);
    rp_recvTypeButtonGroup->addButton(rp_recvTypeHexRadioButton, rp_hexType);
    rp_recvTypeAsciiRadioButton->setChecked(true);


    rp_recvLogModeCheckBox = new QCheckBox(tr("日志模式显示"));
    rp_recvLogModeCheckBox->setStyleSheet("color:white");
    rp_recvLineWrapCheckBox = new QCheckBox(tr("接收区自动换行"));
    rp_recvLineWrapCheckBox->setStyleSheet("color:white");
    rp_recvSaveDataCheckBox = new QCheckBox(tr("保存数据到文件"));
    rp_recvSaveDataCheckBox->setStyleSheet("color:white");
    rp_recvStatePushButton = new QPushButton(tr("暂停接收"));
    rp_recvStatePushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");
    rp_recvClearPushButton = new QPushButton(tr("清空接收区"));
    rp_recvClearPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");

    rp_netRecvSetLayout->addWidget(rp_recvTypeAsciiRadioButton, 0, 0);
    rp_netRecvSetLayout->addWidget(rp_recvTypeHexRadioButton, 0, 1);
    rp_netRecvSetLayout->addWidget(rp_recvLogModeCheckBox, 1, 0, 1, 2);
    rp_netRecvSetLayout->addWidget(rp_recvLineWrapCheckBox, 2, 0, 1, 2);
    rp_netRecvSetLayout->addWidget(rp_recvSaveDataCheckBox, 3, 0, 1, 2);
    rp_netRecvSetLayout->addWidget(rp_recvStatePushButton, 4, 0);
    rp_netRecvSetLayout->addWidget(rp_recvClearPushButton, 4, 1);
    rp_netRecvSetLayout->setColumnStretch(0, 1);
    rp_netRecvSetLayout->setColumnStretch(1, 1);
    rp_netRecvSetGroupBox->setLayout(rp_netRecvSetLayout);

    /* net send set */
    rp_netSendSetGroupBox = new QGroupBox(tr("发送设置"));
    rp_netSendSetGroupBox->setStyleSheet("color:white");
    QGridLayout *rp_netSendSetLayout = new QGridLayout;

    rp_sendTypeButtonGroup = new QButtonGroup(this);
    rp_sendTypeAsciiRadioButton = new QRadioButton(tr("ASCII"));
    rp_sendTypeAsciiRadioButton->setStyleSheet("color:white");
    rp_sendTypeHexRadioButton = new QRadioButton(tr("HEX"));
    rp_sendTypeHexRadioButton->setStyleSheet("color:white");
    rp_sendTypeButtonGroup->addButton(rp_sendTypeAsciiRadioButton, rp_asciiType);
    rp_sendTypeButtonGroup->addButton(rp_sendTypeHexRadioButton, rp_hexType);
    rp_sendTypeAsciiRadioButton->setChecked(true);

    rp_sendPECCheckBox = new QCheckBox(tr("解析转义符"));
    rp_sendPECCheckBox->setStyleSheet("color:white");
    rp_sendLineWrapCheckBox = new QCheckBox(tr("发送区自动换行"));
    rp_sendLineWrapCheckBox->setStyleSheet("color:white");
    rp_sendOpenDataCheckBox = new QCheckBox(tr("打开文件数据"));
    rp_sendOpenDataCheckBox->setStyleSheet("color:white");
    rp_sendCycleCheckBox = new QCheckBox(tr("循环发送"));
    rp_sendCycleCheckBox->setStyleSheet("color:white");
    rp_sendCycleTimeEdit = new QLineEdit(tr("200"));
    rp_sendCycleTimeEdit->setStyleSheet("color:block");
    rp_sendCycleTimeLabel = new QLabel(tr("ms"));
    rp_sendCycleTimeLabel->setStyleSheet("color:white");
    rp_sendClearPushButton = new QPushButton(tr("清空发送区"));
    rp_sendClearPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");

    rp_netSendSetLayout->addWidget(rp_sendTypeAsciiRadioButton, 0, 0);
    rp_netSendSetLayout->addWidget(rp_sendTypeHexRadioButton, 0, 1);
    rp_netSendSetLayout->addWidget(rp_sendPECCheckBox, 1, 0, 1, 2);
    rp_netSendSetLayout->addWidget(rp_sendLineWrapCheckBox, 2, 0, 1, 3);
    rp_netSendSetLayout->addWidget(rp_sendOpenDataCheckBox, 3, 0, 1, 3);
    rp_netSendSetLayout->addWidget(rp_sendCycleCheckBox, 4, 0, 1, 1);
    rp_netSendSetLayout->addWidget(rp_sendCycleTimeEdit, 4, 1, 1, 1);
    rp_netSendSetLayout->addWidget(rp_sendCycleTimeLabel, 4, 2, 1, 1);
    rp_netSendSetLayout->addWidget(rp_sendClearPushButton, 5, 0, 1, 3);
    rp_netSendSetGroupBox->setLayout(rp_netSendSetLayout);

    /* net recv data */
    rp_netReveTabWidget = new QTabWidget;
    QWidget *rp_netRecvWidget = new QWidget;
    QGridLayout *rp_netRecvLayout = new QGridLayout;
    rp_netRecvDataTextEdit = new QTextEdit;
    rp_netRecvLayout->addWidget(rp_netRecvDataTextEdit);
    rp_netRecvWidget->setLayout(rp_netRecvLayout);
    rp_netReveTabWidget->addTab(rp_netRecvWidget, tr("接收日志"));

    /* net send data */
    rp_netSendTabWidget = new QTabWidget;
    QWidget *rp_netsendWidget = new QWidget;
    QGridLayout *rp_netSendLayout = new QGridLayout;
    rp_netSendDataTextEdit = new QTextEdit;
    rp_netSendDataPushButton = new QPushButton(tr("发送"));
    rp_netSendDataTextEdit->adjustSize();
    rp_netSendDataPushButton->setFixedHeight(rp_netSendDataTextEdit->size().rheight()*0.75);
    rp_netSendLayout->addWidget(rp_netSendDataTextEdit, 0, 1, 1, 6);
    rp_netSendLayout->addWidget(rp_netSendDataPushButton,0, 7, 1, 1);
    rp_netsendWidget->setLayout(rp_netSendLayout);
    rp_netSendTabWidget->addTab(rp_netsendWidget, tr("发送数据"));
    rp_netSendDataPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");

    rp_netStateLabel = new QLabel(tr("Ready!"));
    rp_netStateLabel->setStyleSheet("color:white");

    rp_recvFrameLabel = new QLabel(tr("RX Frame"));
    rp_recvFrameLabel->setStyleSheet("color:white");
    rp_recvFrameLineEdit = new QLineEdit(tr("0"));
    rp_recvFrameLineEdit->setStyleSheet("color:block");
    rp_recvByteLabel = new QLabel(tr("Rx Byte"));
    rp_recvByteLabel->setStyleSheet("color:white");
    rp_recvByteLineEdit = new QLineEdit(tr("0"));
    rp_recvByteLineEdit->setStyleSheet("color:block");

    rp_sendFrameLabel = new QLabel(tr("TX Frame"));
    rp_sendFrameLabel->setStyleSheet("color:white");
    rp_sendFrameLineEdit = new QLineEdit(tr("0"));
    rp_sendFrameLineEdit->setStyleSheet("color:block");
    rp_sendByteLabel = new QLabel(tr("TX Byte"));
    rp_sendByteLabel->setStyleSheet("color:white");
    rp_sendByteLineEdit = new QLineEdit(tr("0"));
    rp_sendByteLineEdit->setStyleSheet("color:block");

    rp_resetCountPushButton = new QPushButton(tr("复位计数"));
    rp_resetCountPushButton->setStyleSheet("background:rgb(80, 80, 80);color:white");


    QGridLayout *rp_mainLayout = new QGridLayout;
    //rp_mainLayout->setMargin(80);
    rp_mainLayout->addWidget(rp_netSetGroupBox, 0, 0);
    rp_mainLayout->addWidget(rp_netRecvSetGroupBox, 1, 0);
    rp_mainLayout->addWidget(rp_netSendSetGroupBox, 2, 0);
    rp_mainLayout->addWidget(rp_netStateLabel, 3, 0);
    rp_mainLayout->addWidget(rp_netReveTabWidget, 0, 1, 2, 9);
    rp_mainLayout->addWidget(rp_netSendTabWidget, 2, 1, 1, 9);

    rp_mainLayout->addWidget(rp_recvFrameLabel, 3, 1);
    rp_mainLayout->addWidget(rp_recvFrameLineEdit, 3, 2);
    rp_mainLayout->addWidget(rp_recvByteLabel, 3, 3);
    rp_mainLayout->addWidget(rp_recvByteLineEdit, 3, 4);
    rp_mainLayout->addWidget(rp_sendFrameLabel, 3, 5);
    rp_mainLayout->addWidget(rp_sendFrameLineEdit, 3, 6);
    rp_mainLayout->addWidget(rp_sendByteLabel, 3, 7);
    rp_mainLayout->addWidget(rp_sendByteLineEdit, 3, 8);
    rp_mainLayout->addWidget(rp_resetCountPushButton, 3, 9);

    rp_mainLayout->setColumnStretch(0, 3);
    rp_mainLayout->setColumnStretch(1, 9);
    rp_mainLayout->setRowStretch(0, 8);
    rp_mainLayout->setRowStretch(1, 6);
    rp_mainLayout->setRowStretch(2, 7);
    rp_mainLayout->setRowStretch(3, 1);

    setLayout(rp_mainLayout);

}

void rp_net_aid::rp_getLocalIpAddr(void)
{
    QList<QHostAddress> rp_localIpAddrlist = QNetworkInterface::allAddresses();
    foreach(QHostAddress localIpAddr, rp_localIpAddrlist)
    {
        if(localIpAddr.protocol() == QAbstractSocket::IPv4Protocol && (localIpAddr == QHostAddress::LocalHost || localIpAddr.toString().left(3) != "169"))
        {
            rp_localAddrComboBox->addItem(localIpAddr.toString());
        }
    }
}

void rp_net_aid::rp_netAidFuncConfig()
{
    rp_getLocalIpAddr();

    connect(rp_recvTypeButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(rp_netRecvTpyeCheck(int,bool)));
    connect(rp_sendTypeButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(rp_netSendTpyeCheck(int,bool)));

    rp_netAidLanguageType = true;
}

void rp_net_aid::rp_netRecvTpyeCheck(int id, bool status)
{
    Q_UNUSED(id);
    Q_UNUSED(status);
    qDebug() << rp_recvTypeButtonGroup->checkedId();
}

void rp_net_aid::rp_netSendTpyeCheck(int id, bool status)
{
    Q_UNUSED(id);
    Q_UNUSED(status);
    qDebug() << rp_sendTypeButtonGroup->checkedId();
}

bool rp_net_aid::rp_netAidGetLanguageType()
{
    return rp_netAidLanguageType;
}

void rp_net_aid::rp_netAidLSetLanguageType(bool type)
{
    rp_netAidLanguageType = type;

    if(rp_netAidLanguageType)
    {
        rp_netSetGroupBox->setTitle(tr("网络设置"));
        rp_protocolTypeLabel->setText(tr("(1) 协议类型"));
        rp_localAddrLabel->setText(tr("(2) 本地主机地址"));
        rp_localPortLabel->setText(tr("(3) 本地主机端口"));
        rp_switchNetPushButton->setText(tr("打开"));

        rp_netRecvSetGroupBox->setTitle(tr("接受设置"));
        rp_recvLogModeCheckBox->setText(tr("日志模式显示"));
        rp_recvLineWrapCheckBox->setText(tr("接收区自动换行"));
        rp_recvSaveDataCheckBox->setText(tr("保存数据到文件"));
        rp_recvStatePushButton->setText(tr("暂停接收"));
        rp_recvClearPushButton->setText(tr("清空接收区"));

        rp_netSendSetGroupBox->setTitle(tr("接受设置"));
        rp_sendPECCheckBox->setText(tr("解析转义符"));
        rp_sendLineWrapCheckBox->setText(tr("发送区自动换行"));
        rp_sendOpenDataCheckBox->setText(tr("打开文件数据"));
        rp_sendCycleCheckBox->setText(tr("循环发送"));
        rp_sendClearPushButton->setText(tr("清空发送区"));

        rp_netReveTabWidget->setTabText(0, tr("接收日志"));
        rp_netSendTabWidget->setTabText(0, tr("发送数据"));
        rp_netSendDataPushButton->setText(tr("发送"));
        rp_resetCountPushButton->setText(tr("复位计数"));
    }
    else
    {
        rp_netSetGroupBox->setTitle(tr("Net Setting"));
        rp_protocolTypeLabel->setText(tr("(1) Protocol Type"));
        rp_localAddrLabel->setText(tr("(2) Local Addr"));
        rp_localPortLabel->setText(tr("(3) Local Port"));
        rp_switchNetPushButton->setText(tr("Open"));

        rp_netRecvSetGroupBox->setTitle(tr("Recv Setting"));
        rp_recvLogModeCheckBox->setText(tr("Log mode show"));
        rp_recvLineWrapCheckBox->setText(tr("Auto Line Wrap"));
        rp_recvSaveDataCheckBox->setText(tr("Save Data to File"));
        rp_recvStatePushButton->setText(tr("Stop Recv"));
        rp_recvClearPushButton->setText(tr("Clear"));

        rp_netSendSetGroupBox->setTitle(tr("Send Setting"));
        rp_sendPECCheckBox->setText(tr("Resolve Escape Characters"));
        rp_sendLineWrapCheckBox->setText(tr("Auto Line Wrap"));
        rp_sendOpenDataCheckBox->setText(tr("Get File Data"));
        rp_sendCycleCheckBox->setText(tr("Cycle Send"));
        rp_sendClearPushButton->setText(tr("Clear"));

        rp_netReveTabWidget->setTabText(0, tr("Recv Log"));
        rp_netSendTabWidget->setTabText(0, tr("Send Data"));
        rp_netSendDataPushButton->setText(tr("Send"));
        rp_resetCountPushButton->setText(tr("Reset Count"));
    }
}
