#ifndef RP_NET_AID_H
#define RP_NET_AID_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>

class rp_net_aid : public QWidget
{
    Q_OBJECT
public:
    explicit rp_net_aid(QWidget *parent = 0);

private:
    void rp_netAidUI();
    void rp_netAidFuncConfig();

public:
    bool rp_netAidGetLanguageType();     // //TRUE = chinese, FALSE = english
    void rp_netAidLSetLanguageType(bool type);

private:
    QGroupBox *rp_netSetGroupBox;
    QLabel *rp_protocolTypeLabel;
    QComboBox * rp_protocolTypeComboBox;
    QLabel *rp_localAddrLabel;
    QComboBox * rp_localAddrComboBox;
    QLabel *rp_localPortLabel;
    QLineEdit * rp_localPortLineEdit;
    QPushButton *rp_switchNetPushButton;

    QGroupBox *rp_netRecvSetGroupBox;
    QRadioButton *rp_recvTypeAsciiRadioButton;
    QRadioButton *rp_recvTypeHexRadioButton;
    QCheckBox *rp_recvLogModeCheckBox;
    QCheckBox *rp_recvLineWrapCheckBox;
    QCheckBox *rp_recvSaveDataCheckBox;
    QPushButton *rp_recvStatePushButton;
    QPushButton *rp_recvClearPushButton;

    QGroupBox *rp_netSendSetGroupBox;
    QRadioButton *rp_sendTypeAsciiRadioButton;
    QRadioButton *rp_sendTypeHexRadioButton;
    QCheckBox *rp_sendPECCheckBox;
    QCheckBox *rp_sendLineWrapCheckBox;
    QCheckBox *rp_sendOpenDataCheckBox;
    QCheckBox *rp_sendCycleCheckBox;
    QLineEdit *rp_sendCycleTimeEdit;
    QLabel *rp_sendCycleTimeLabel;
    QPushButton *rp_sendClearPushButton;

    QTabWidget *rp_netReveTabWidget;
    QTextEdit *rp_netRecvDataTextEdit;

    QTabWidget *rp_netSendTabWidget;
    QTextEdit *rp_netSendDataTextEdit;
    QPushButton *rp_netSendDataPushButton;

    QLabel *rp_netStateLabel;

    QLabel *rp_recvFrameLabel;
    QLineEdit *rp_recvFrameLineEdit;
    QLabel *rp_sendFrameLabel;
    QLineEdit *rp_sendFrameLineEdit;
    QLabel *rp_recvByteLabel;
    QLineEdit *rp_recvByteLineEdit;
    QLabel *rp_sendByteLabel;
    QLineEdit *rp_sendByteLineEdit;

    QPushButton *rp_resetCountPushButton;

    bool rp_netAidLanguageType;

};

#endif // RP_NET_AID_H
