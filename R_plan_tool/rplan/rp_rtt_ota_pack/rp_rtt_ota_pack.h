#ifndef RRP_RTT_OTA_PACK_H
#define RRP_RTT_OTA_PACK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QComboBox>
#include <QFileDialog>

enum rp_rt_ota_algo
{
    RP_RT_OTA_CRYPT_ALGO_NONE    = 0x0L,               /**< no encryption algorithm and no compression algorithm */
    RP_RT_OTA_CRYPT_ALGO_XOR     = 0x1L,               /**< XOR encryption */
    RP_RT_OTA_CRYPT_ALGO_AES256  = 0x2L,               /**< AES256 encryption */
    RP_RT_OTA_CMPRS_ALGO_GZIP    = 0x1L << 8,          /**< Gzip: zh.wikipedia.org/wiki/Gzip */
    RP_RT_OTA_CMPRS_ALGO_QUICKLZ = 0x2L << 8,          /**< QuickLZ: www.quicklz.com */
    RP_RT_OTA_CMPRS_ALGO_FASTLZ  = 0x3L << 8,          /**< FastLZ: fastlz.org/ */

    RP_RT_OTA_CRYPT_STAT_MASK    = 0xFL,
    RP_RT_OTA_CMPRS_STAT_MASK    = 0xFL << 8,
};
typedef enum rp_rt_ota_algo rp_rt_ota_algo_t;

struct rp_rt_ota_rbl_hdr
{
    char magic[4];

    rp_rt_ota_algo_t algo;
    unsigned int timestamp;
    char name[16];
    char version[24];

    char sn[24];

    unsigned int crc32;
    unsigned int hash;

    unsigned int size_raw;
    unsigned int size_package;

    unsigned int info_crc32;
};

class rp_rtt_ota_pack : public QWidget
{
    Q_OBJECT
public:
    explicit rp_rtt_ota_pack(QWidget *parent = 0);

private:
    void rp_rttOtaPackUI();
    void rp_rttOtaFuncConfig();

private:
    QPushButton *rp_selectFwButton;
    QLineEdit *rp_selectFwLineEdit;
    QPushButton *rp_savePathButton;
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

    QPushButton *rp_packButton;

    QFileDialog *rp_openFileDialog;
    QFileDialog *rp_saveFileDialog;

private:
    struct rp_rt_ota_rbl_hdr rp_rblHander;

private slots:
    void rp_selectFwButtonHandle();
    void rp_savePathButtonHandle();
    void rp_packButtonHandle();

signals:
};

#endif // RRP_RTT_OTA_PACK_H
