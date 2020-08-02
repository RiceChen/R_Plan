#ifndef RRP_RTT_OTA_PACK_H
#define RRP_RTT_OTA_PACK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QComboBox>

class rp_rtt_ota_pack : public QWidget
{
    Q_OBJECT
public:
    explicit rp_rtt_ota_pack(QWidget *parent = 0);

private:
    void rp_rttOtaPackUI();

private:
    QLineEdit *rp_selectFwLineEdit;
    QLineEdit *rp_savePathLineEdit;
    QComboBox *rp_comAlgComboBox;
    QComboBox *rp_encAlgComboBox;
    QLineEdit *rp_priKeyLineEdit;
    QLineEdit *rp_encIvLineEdit;
    QLineEdit *rp_fwNameLineEdit;
    QLineEdit *rp_fwVerLineEdit;

    QLabel *rp_hashCodeResultLabel;
    QLabel *rp_rawSizeResultLabel;
    QLabel *rp_hdrCrc32ResultLabel;
    QLabel *rp_pkgSizeResultLabel;
    QLabel *rp_bodyCrc32ResultLabel;
    QLabel *rp_timestampResultLabel;

signals:
};

#endif // RRP_RTT_OTA_PACK_H
