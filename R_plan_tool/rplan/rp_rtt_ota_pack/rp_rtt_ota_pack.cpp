#include "rp_rtt_ota_pack.h"

#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>
#include <QCoreApplication>
#include <QtEndian>

rp_rtt_ota_pack::rp_rtt_ota_pack(QWidget *parent)
    : QWidget(parent)
{
    rp_rttOtaPackUI();

    rp_rttOtaFuncConfig();
}

void rp_rtt_ota_pack::rp_rttOtaPackUI()
{
    /* Logo */
    QLabel *rp_rttLogoLabel = new QLabel(tr(""));
    QFont rp_rttLogoFont("Microsoft YaHei", 10, 75);
    rp_rttLogoLabel->setFont(rp_rttLogoFont);

    QPixmap rp_rttlogoPixmap(":/image/rtt_logo.png");
    int rp_rttLogoWidth = rp_rttLogoLabel->width();
    int rp_rttLogoHight = rp_rttLogoLabel->height();
    rp_rttlogoPixmap.scaled(rp_rttLogoWidth, rp_rttLogoHight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    rp_rttLogoLabel->setPixmap(rp_rttlogoPixmap);

    QLabel *rp_rttOtaNameLable = new QLabel(tr("   RT-Thread OTA 固件打包工具"));
    QFont rp_rttOtaNameFont("Microsoft YaHei", 20, 75);
    rp_rttOtaNameLable->setFont(rp_rttOtaNameFont);

    /* func */
    QFont rp_funcInfoFont("Microsoft YaHei", 15);

    // select FW
    rp_selectFwButton = new QPushButton(tr("选择固件"));
    rp_selectFwButton->setFixedSize(160, 50);
    rp_selectFwLineEdit = new QLineEdit();
    rp_selectFwLineEdit->setFont(rp_funcInfoFont);

    //svae path
    rp_savePathButton = new QPushButton(tr("保存路径"));
    rp_savePathButton->setFixedSize(160, 50);
    rp_savePathLineEdit = new QLineEdit();
    rp_savePathLineEdit->setFont(rp_funcInfoFont);

    // compression algorithm
    QLabel *rp_comAlgLable = new QLabel(tr("压缩算法"));
    rp_comAlgLable->setFont(rp_funcInfoFont);
    rp_comAlgComboBox = new QComboBox();
    rp_comAlgComboBox->setFont(rp_funcInfoFont);
    QStringList rp_comAlgStrList;
    rp_comAlgStrList << "不压缩" << "quicklz" << "fastlz" << "gzip";
    rp_comAlgComboBox->addItems(rp_comAlgStrList);
    rp_comAlgComboBox->setCurrentIndex(3);

    //encryption algorithm
    QLabel *rp_encAlgLable = new QLabel(tr("加密算法"));
    rp_encAlgLable->setFont(rp_funcInfoFont);
    rp_encAlgComboBox = new QComboBox();
    rp_encAlgComboBox->setFont(rp_funcInfoFont);
    QStringList rp_encAlgStrList;
    rp_encAlgStrList << "不加密" << "AES256";
    rp_encAlgComboBox->addItems(rp_encAlgStrList);
    rp_encAlgComboBox->setCurrentIndex(1);

    //private key
    QLabel *rp_priKeyLable = new QLabel(tr("加密秘钥"));
    rp_priKeyLable->setFont(rp_funcInfoFont);
    rp_priKeyLineEdit = new QLineEdit(tr("0123456789ABCDEF0123456789ABCDEF"));
    rp_priKeyLineEdit->setFont(rp_funcInfoFont);

    //encryption IV
    QLabel *rp_encIvLable = new QLabel(tr("加密 IV"));
    rp_encIvLable->setFont(rp_funcInfoFont);
    rp_encIvLineEdit = new QLineEdit(tr("0123456789ABCDEF"));
    rp_encIvLineEdit->setFont(rp_funcInfoFont);

    //FW name
    QLabel *rp_fwNameLable = new QLabel(tr("固件名称"));
    rp_fwNameLable->setFont(rp_funcInfoFont);
    rp_fwNameLineEdit = new QLineEdit(tr("app"));
    rp_fwNameLineEdit->setFont(rp_funcInfoFont);

    // FW version
    QLabel *rp_fwVerLable = new QLabel(tr("固件版本"));
    rp_fwVerLable->setFont(rp_funcInfoFont);
    rp_fwVerLineEdit = new QLineEdit(tr("2.5"));
    rp_fwVerLineEdit->setFont(rp_funcInfoFont);

    QGridLayout *rp_funcLayout = new QGridLayout;
    rp_funcLayout->addWidget(rp_rttLogoLabel, 0, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_rttOtaNameLable, 0, 1, 1, 3);
    rp_funcLayout->addWidget(rp_selectFwButton, 1, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_selectFwLineEdit, 1, 1, 1, 3);
    rp_funcLayout->addWidget(rp_savePathButton, 2, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_savePathLineEdit, 2, 1, 1, 3);
    rp_funcLayout->addWidget(rp_comAlgLable, 3, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_comAlgComboBox, 3, 1, 1, 3);
    rp_funcLayout->addWidget(rp_encAlgLable, 4, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_encAlgComboBox, 4, 1, 1, 3);
    rp_funcLayout->addWidget(rp_priKeyLable, 5, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_priKeyLineEdit, 5, 1, 1, 3);
    rp_funcLayout->addWidget(rp_encIvLable, 6, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_encIvLineEdit, 6, 1, 1, 3);
    rp_funcLayout->addWidget(rp_fwNameLable, 7, 0, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_fwNameLineEdit, 7, 1);
    rp_funcLayout->addWidget(rp_fwVerLable, 7, 2, Qt::AlignHCenter);
    rp_funcLayout->addWidget(rp_fwVerLineEdit, 7, 3);

    /* result */

    QString rp_resultInfoColor = "color:Red";
    QLabel *rp_resultLable = new QLabel(tr("结果:"));
    QFont rp_resultFont("Microsoft YaHei", 15);
    rp_resultLable->setFont(rp_resultFont);
    rp_resultLable->setStyleSheet(rp_resultInfoColor);

    QFont rp_resultInfoFont("Microsoft YaHei", 12);
    rp_resultInfoColor = "color:gray";
    // hash code
    QLabel *rp_hashCodeLable = new QLabel(tr("HASH_CODE:"));
    rp_hashCodeLable->setFont(rp_resultInfoFont);
    rp_hashCodeLable->setStyleSheet(rp_resultInfoColor);
    rp_hashCodeResultLabel = new QLabel();
    rp_hashCodeResultLabel->setFont(rp_resultInfoFont);
    rp_hashCodeResultLabel->setStyleSheet(rp_resultInfoColor);

    // raw size
    QLabel *rp_rawSizeLable = new QLabel(tr("RAW_SIZE:"));
    rp_rawSizeLable->setFont(rp_resultInfoFont);
    rp_rawSizeLable->setStyleSheet(rp_resultInfoColor);
    rp_rawSizeResultLabel = new QLabel();
    rp_rawSizeResultLabel->setFont(rp_resultInfoFont);
    rp_rawSizeResultLabel->setStyleSheet(rp_resultInfoColor);

    // hdr crc32
    QLabel *rp_hdrCrc32Lable = new QLabel(tr("HDR_CRC32:"));
    rp_hdrCrc32Lable->setFont(rp_resultInfoFont);
    rp_hdrCrc32Lable->setStyleSheet(rp_resultInfoColor);
    rp_hdrCrc32ResultLabel = new QLabel();
    rp_hdrCrc32ResultLabel->setFont(rp_resultInfoFont);
    rp_hdrCrc32ResultLabel->setStyleSheet(rp_resultInfoColor);

    // pkg size
    QLabel *rp_pkgSizeLable = new QLabel(tr("PKG_SIZE:"));
    rp_pkgSizeLable->setFont(rp_resultInfoFont);
    rp_pkgSizeLable->setStyleSheet(rp_resultInfoColor);
    rp_pkgSizeResultLabel = new QLabel();
    rp_pkgSizeResultLabel->setFont(rp_resultInfoFont);
    rp_pkgSizeResultLabel->setStyleSheet(rp_resultInfoColor);

    // body crc32
    QLabel *rp_bodyCrc32Lable = new QLabel(tr("BODY_CRC32:"));
    rp_bodyCrc32Lable->setFont(rp_resultInfoFont);
    rp_bodyCrc32Lable->setStyleSheet(rp_resultInfoColor);
    rp_bodyCrc32ResultLabel = new QLabel();
    rp_bodyCrc32ResultLabel->setFont(rp_resultInfoFont);
    rp_bodyCrc32ResultLabel->setStyleSheet(rp_resultInfoColor);

    // body crc32
    QLabel *rp_timestampLable = new QLabel(tr("TIMESTAMP:"));
    rp_timestampLable->setFont(rp_resultInfoFont);
    rp_timestampLable->setStyleSheet(rp_resultInfoColor);
    rp_timestampResultLabel = new QLabel();
    rp_timestampResultLabel->setFont(rp_resultInfoFont);
    rp_timestampResultLabel->setStyleSheet(rp_resultInfoColor);

    // pack button
    rp_packButton = new QPushButton(tr("开始打包"));
    rp_packButton->setFont(rp_resultInfoFont);
    rp_packButton->setFixedSize(160, 100);

    QGridLayout *rp_resultLayout = new QGridLayout;
    rp_resultLayout->addWidget(rp_resultLable, 0, 0, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_hashCodeLable, 1, 0, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_hashCodeResultLabel, 1, 1, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_rawSizeLable, 1, 2, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_rawSizeResultLabel, 1, 3, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_hdrCrc32Lable, 2, 0, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_hdrCrc32ResultLabel, 2, 1, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_pkgSizeLable, 2, 2, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_pkgSizeResultLabel, 2, 3, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_bodyCrc32Lable, 3, 0, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_bodyCrc32ResultLabel, 3, 1, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_timestampLable, 3, 2, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_timestampResultLabel, 3, 3, Qt::AlignLeft);
    rp_resultLayout->addWidget(rp_packButton, 1, 4, 3, 1);
    rp_resultLayout->setMargin(20);

    QVBoxLayout *rp_mainLayout = new QVBoxLayout;
    rp_mainLayout->setMargin(60);
    rp_mainLayout->addLayout(rp_funcLayout, Qt::AlignVCenter);
    rp_mainLayout->addLayout(rp_resultLayout, Qt::AlignVCenter);

    setLayout(rp_mainLayout);
}

void rp_rtt_ota_pack::rp_rttOtaFuncConfig()
{
    rp_openFileDialog = new QFileDialog();
    rp_openFileDialog->setWindowTitle("R-Plan select RTT fireware");
    rp_openFileDialog->setDirectory(".");
    rp_openFileDialog->setNameFilter(tr("File(*.bin)"));
    rp_openFileDialog->setViewMode(QFileDialog::Detail);

    rp_selectFwLineEdit->setText(tr("rtthread.bin"));
    rp_savePathLineEdit->setText(tr("rtthread.rbl"));

    connect(rp_selectFwButton, SIGNAL(clicked()), this, SLOT(rp_selectFwButtonHandle()));
    connect(rp_savePathButton, SIGNAL(clicked()), this, SLOT(rp_savePathButtonHandle()));
    connect(rp_packButton, SIGNAL(clicked()), this, SLOT(rp_packButtonHandle()));
}

void rp_rtt_ota_pack::rp_selectFwButtonHandle()
{
    qDebug() << "Into select Fw!" << endl;
    QString rp_fwFileName;
    QString rp_saveFileName;
    if(QDialog::Accepted == rp_openFileDialog->exec())
    {
        rp_fwFileName = rp_openFileDialog->selectedFiles()[0];
        rp_selectFwLineEdit->setText(rp_fwFileName);
        rp_saveFileName = rp_fwFileName.left(rp_fwFileName.length() - 3) + "rbl";
        rp_savePathLineEdit->setText(rp_saveFileName);
        qDebug() << rp_fwFileName << endl;
    }
}

void rp_rtt_ota_pack::rp_savePathButtonHandle()
{
    qDebug() << "Into save path!" << endl;
    rp_saveFileDialog = new QFileDialog();

    QString rp_saveFileName = rp_saveFileDialog->getSaveFileName(this,
                                                          tr("R-Plan save RTT fireware"),
                                                          tr("rtthread"),
                                                          tr("*.rbl"));
    if(!rp_saveFileName.isEmpty())
    {
        rp_savePathLineEdit->setText(rp_saveFileName);
    }
}

void rp_rtt_ota_pack::rp_packButtonHandle()
{
    qDebug() << "Into pack!" << endl;
    QProcess *rp_rttOtaPackProcess = new QProcess();
    QString rp_program = QCoreApplication::applicationDirPath() + "/tools/rt_ota_packaging_tool/rt_ota_packaging_tool_cli.exe";
    QStringList rp_arguments;

    // source file
    if(!rp_selectFwLineEdit->text().isEmpty())
    {
        rp_arguments << "-f" << rp_selectFwLineEdit->text();
    }
    else
    {
        QMessageBox::about(this, tr("Warning"),
                           tr("Please select source file!"));
        return;
    }

    // version
    if(!rp_fwVerLineEdit->text().isEmpty())
    {
        rp_arguments << "-v" << rp_fwVerLineEdit->text();
    }
    else
    {
        QDateTime rp_currentTime = QDateTime::currentDateTime();
        QString rp_currentTimeStr = rp_currentTime.toString("yyyyMMdd_hhmmss");

        rp_arguments << "-v" << rp_currentTimeStr;
    }

    // partition name
    if(!rp_fwNameLineEdit->text().isEmpty())
    {
        rp_arguments << "-p" << rp_fwNameLineEdit->text();
    }
    else
    {
        rp_arguments << "-p" << "app";
    }

    // target file
    if(!rp_savePathLineEdit->text().isEmpty())
    {
        rp_arguments << "-o" << rp_savePathLineEdit->text();
    }
    else
    {
        QMessageBox::about(this, tr("Warning"),
                           tr("Please select save path!"));
        return;
    }

    // compression algorithm
    if(rp_comAlgComboBox->currentText() != tr("不压缩"))
    {
        rp_arguments << "-c" << rp_comAlgComboBox->currentText();
    }

    // encryption algorithm
    if(rp_encAlgComboBox->currentText() != tr("不加密"))
    {
        rp_arguments << "-s" << "aes";
    }

    // encryption IV
    if(!rp_encIvLineEdit->text().isEmpty())
    {
        rp_arguments << "-i" << rp_encIvLineEdit->text();
    }
    else
    {
        rp_arguments << "-i" << "0123456789ABCDEF";
    }

    // private key
    if(!rp_priKeyLineEdit->text().isEmpty())
    {
        rp_arguments << "-k" << rp_priKeyLineEdit->text();
    }
    else
    {
        rp_arguments << "-k" << "0123456789ABCDEF0123456789ABCDEF";
    }

    // generate rbl
    qDebug() << rp_program << rp_arguments << endl;
    rp_rttOtaPackProcess->start(rp_program, rp_arguments);

    QFile rp_rblFile(rp_savePathLineEdit->text());
    if(!rp_rblFile.exists())
    {
        qDebug() << "RBL file does not exist!" << endl;
        return;
    }
    if(!rp_rblFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open RBL file!" << endl;
        return;
    }
    QByteArray rp_rblFileContent = rp_rblFile.readAll();
    char *rp_rblHanderChar = rp_rblFileContent.data();

    memcpy(&rp_rblHander, rp_rblHanderChar, sizeof(struct rp_rt_ota_rbl_hdr));

    rp_hashCodeResultLabel->setText(QString::number(qFromLittleEndian(rp_rblHander.hash), 16));
    rp_rawSizeResultLabel->setText(QString::number(rp_rblHander.size_raw));
    rp_hdrCrc32ResultLabel->setText(QString::number(qFromLittleEndian(rp_rblHander.info_crc32), 16));
    rp_pkgSizeResultLabel->setText(QString::number(rp_rblHander.size_package));
    rp_bodyCrc32ResultLabel->setText(QString::number(qFromLittleEndian(rp_rblHander.crc32), 16));
    rp_timestampResultLabel->setText(QString::number(rp_rblHander.timestamp));
    rp_rblFile.close();
}

