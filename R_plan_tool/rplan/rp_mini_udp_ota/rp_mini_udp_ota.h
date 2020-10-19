#ifndef RP_MINI_UDP_OTA_H
#define RP_MINI_UDP_OTA_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>

class rp_mini_udp_ota : public QWidget
{
    Q_OBJECT
public:
    rp_mini_udp_ota(QWidget *parent = 0);
    ~rp_mini_udp_ota();

private:
    void rp_udpOtaWindowUI();
    void rp_udpOtaFuncConfig();


private:
    QLineEdit *rp_fwPathLineEdit;
    QPushButton *rp_fwSelectPushButton;

    QTableView *rp_scanResultTableView;
};

#endif // RP_MINI_UDP_OTA_H
