#include "rp_find_file.h"

#include <QtWidgets>
#include <QGridLayout>

rp_find_file::rp_find_file(QWidget *parent)
    : QWidget(parent)
{
    rp_findFileWindowUI();

    rp_findFileFuncConfig();
}

rp_find_file::~rp_find_file()
{
    ;
}

void rp_find_file::rp_findFileWindowUI()
{
    rp_fileNameComboBox = rp_createComboBox(tr("*"));
    rp_findContentComboBox = rp_createComboBox();
    rp_directoryComboBox = rp_createComboBox(QDir::toNativeSeparators(QDir::currentPath()));

    rp_browsePushButton = new QPushButton(tr("Browse"));

    rp_filesFoundLabel = new QLabel;
    rp_filesFoundLabel->setText(tr("%n file(s) found (Double click on a file to open it)", 0, 0));

    rp_createFilesTable();

    rp_findPushButton = new QPushButton(tr("Find"));

    QGridLayout *rp_mainLayout = new QGridLayout(this);
    rp_mainLayout->addWidget(new QLabel(tr("Find File name:")), 0, 0);
    rp_mainLayout->addWidget(rp_fileNameComboBox, 0, 1, 1, 2);
    rp_mainLayout->addWidget(new QLabel(tr("Find content:")), 1, 0);
    rp_mainLayout->addWidget(rp_findContentComboBox, 1, 1, 1, 2);
    rp_mainLayout->addWidget(new QLabel(tr("Directory:")), 2, 0);
    rp_mainLayout->addWidget(rp_directoryComboBox, 2, 1);
    rp_mainLayout->addWidget(rp_browsePushButton, 2, 2);

    rp_mainLayout->addWidget(rp_fileListTable, 3, 0, 1, 3);
    rp_mainLayout->addWidget(rp_filesFoundLabel, 4, 0, 1, 2);
    rp_mainLayout->addWidget(rp_findPushButton, 4, 2);

    setLayout(rp_mainLayout);
}

void rp_find_file::rp_findFileFuncConfig()
{
    connect(rp_fileNameComboBox->lineEdit(), &QLineEdit::returnPressed, this, &rp_find_file::rp_animateFindClick);
    connect(rp_findContentComboBox->lineEdit(), &QLineEdit::returnPressed, this, &rp_find_file::rp_animateFindClick);
    connect(rp_directoryComboBox->lineEdit(), &QLineEdit::returnPressed, this, &rp_find_file::rp_animateFindClick);

    connect(rp_browsePushButton, SIGNAL(clicked()), this, SLOT(rp_browse()));
    connect(rp_findPushButton, SIGNAL(clicked()), this, SLOT(rp_findFile()));

    connect(rp_fileListTable, &QTableWidget::customContextMenuRequested,
            this, &rp_find_file::rp_comtextMenu);
    connect(rp_fileListTable, &QTableWidget::cellActivated,
            this, &rp_find_file::rp_openFileOfItem);
}

QComboBox *rp_find_file::rp_createComboBox(const QString &text)
{
    QComboBox *rp_comboBox = new QComboBox;
    rp_comboBox->setEditable(true);
    rp_comboBox->addItem(text);
    rp_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return rp_comboBox;
}

void rp_find_file::rp_updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
    {
        comboBox->addItem(comboBox->currentText());
    }
}

void rp_find_file::rp_createFilesTable()
{
    QStringList rp_fileInfo;
    rp_fileListTable = new QTableWidget(0, 2);
    rp_fileListTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    rp_fileInfo << tr("File name") << tr("Size");
    rp_fileListTable->setHorizontalHeaderLabels(rp_fileInfo);
    rp_fileListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    rp_fileListTable->verticalHeader()->hide();
    rp_fileListTable->setShowGrid(false);

    rp_fileListTable->setContextMenuPolicy(Qt::CustomContextMenu);
}

enum { absoluteFileNameRole = Qt::UserRole + 1 };

void rp_find_file::rp_browse()
{
    QString rp_directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath()));

    if(!rp_directory.isEmpty())
    {
        if(rp_directoryComboBox->findText(rp_directory) == -1)
        {
            rp_directoryComboBox->addItem(rp_directory);
        }
        rp_directoryComboBox->setCurrentIndex(rp_directoryComboBox->findText(rp_directory));
    }
}

void rp_find_file::rp_findFile()
{
    QString rp_fileName = rp_fileNameComboBox->currentText();
    QString rp_findContent = rp_findContentComboBox->currentText();
    QString rp_directory = QDir::cleanPath(rp_directoryComboBox->currentText());
    QStringList rp_files;

    rp_fileListTable->setRowCount(0);

    rp_updateComboBox(rp_fileNameComboBox);
    rp_updateComboBox(rp_findContentComboBox);
    rp_updateComboBox(rp_directoryComboBox);

    if(rp_fileName.isEmpty())
    {
        rp_fileName = QStringLiteral("*");
    }
    rp_currentDir = QDir(rp_directory);
    rp_recursionFindFile(rp_directory, rp_fileName, &rp_files);
    if(!rp_findContent.isEmpty())
    {
        rp_files = rp_findContentFile(rp_files, rp_findContent);
    }
    rp_showFiles(rp_files);
}

void rp_find_file::rp_animateFindClick()
{
    rp_findPushButton->animateClick();
}

void rp_find_file::rp_openFileOfItem(int row, int column)
{
    Q_UNUSED(column);
    const QTableWidgetItem *rp_fileItem = rp_fileListTable->item(row, 0);
    const QString rp_fileNameOfItem = rp_fileItem->data(absoluteFileNameRole).toString();

    QDesktopServices::openUrl(QUrl::fromLocalFile(rp_fileNameOfItem));
}

void rp_find_file::rp_comtextMenu(const QPoint &pos)
{
    const QTableWidgetItem *rp_fileItem = rp_fileListTable->itemAt(pos);
    if(!rp_fileItem)
    {
        return;
    }
    QMenu rp_menu;
#ifndef QT_NO_CLIPBOARD
    QAction *rp_copyAction = rp_menu.addAction("Copy Name");
#endif
    QAction *rp_openAction = rp_menu.addAction("Open");
    QAction *rp_action = rp_menu.exec(rp_fileListTable->mapToGlobal(pos));
    if (!rp_action)
        return;
    const QString rp_fileNameOfItem = rp_fileItem->data(absoluteFileNameRole).toString();
    if (rp_action == rp_openAction)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(rp_fileNameOfItem));
    }
#ifndef QT_NO_CLIPBOARD
    else if (rp_action == rp_copyAction)
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(rp_fileNameOfItem));
#endif
}

void rp_find_file::rp_recursionFindFile(const QString &path, const QString &pattern, QStringList *files)
{
    QDir rp_currentDir(path);
    const QString rp_prtfix = path+QLatin1Char('/');

    foreach (const QString &match, rp_currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks))
    {
        files->append(rp_prtfix + match);
    }
    foreach (const QString &dir, rp_currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
    {
        rp_recursionFindFile(rp_prtfix + dir, pattern, files);
    }
}

QStringList rp_find_file::rp_findContentFile(const QStringList &files, const QString &text)
{
    QProgressDialog rp_progressDialog(this);
    QMimeDatabase rp_mimeDatabase;
    QStringList rp_foundFiles;

    rp_progressDialog.setCancelButtonText(tr("&Cancel"));
    rp_progressDialog.setRange(0, files.size());
    rp_progressDialog.setWindowTitle(tr("Find Files"));

    for(int count = 0; count < files.size(); ++count)
    {
        QCoreApplication::processEvents();
        rp_progressDialog.setValue(count);
        rp_progressDialog.setLabelText(tr("Searching file number %1 of %n...", 0, files.size()).arg(count));

        if(rp_progressDialog.wasCanceled())
        {
            break;
        }

        const QString rp_fileName = files.at(count);
        const QMimeType rp_mimeType = rp_mimeDatabase.mimeTypeForFile(rp_fileName);
        if (rp_mimeType.isValid() && !rp_mimeType.inherits(QStringLiteral("text/plain")))
        {
            qWarning() << "Not searching binary file " << QDir::toNativeSeparators(rp_fileName);
            continue;
        }

        QFile rp_file(rp_fileName);
        if (rp_file.open(QIODevice::ReadOnly)) {
            QString rp_line;
            QTextStream rp_in(&rp_file);
            while (!rp_in.atEnd()) {
                if (rp_progressDialog.wasCanceled())
                    break;
                rp_line = rp_in.readLine();
                if (rp_line.contains(text, Qt::CaseInsensitive)) {
                    rp_foundFiles << files[count];
                    break;
                }
            }
        }

    }

    return rp_foundFiles;
}

void rp_find_file::rp_showFiles(const QStringList &files)
{
    for(int count = 0; count < files.size(); ++count)
    {
        const QString &rp_fileName = files.at(count);
        const QString rp_toolTip = QDir::toNativeSeparators(rp_fileName);
        const QString rp_relativePath = QDir::toNativeSeparators(rp_currentDir.relativeFilePath(rp_fileName));
        const qint64 rp_size = QFileInfo(rp_fileName).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(rp_relativePath);
        fileNameItem->setData(absoluteFileNameRole, QVariant(rp_fileName));
        fileNameItem->setToolTip(rp_toolTip);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((rp_size + 1023) / 1024)));
        sizeItem->setData(absoluteFileNameRole, QVariant(rp_fileName));
        sizeItem->setToolTip(rp_toolTip);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int rp_row = rp_fileListTable->rowCount();
        rp_fileListTable->insertRow(rp_row);
        rp_fileListTable->setItem(rp_row, 0, fileNameItem);
        rp_fileListTable->setItem(rp_row, 1, sizeItem);
    }
    rp_filesFoundLabel->setText(tr("%n file(s) found (Double click on a file to open it)", 0, files.size()));
    rp_filesFoundLabel->setWordWrap(true);
}
