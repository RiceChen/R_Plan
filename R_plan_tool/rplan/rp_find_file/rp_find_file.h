#ifndef RP_FIND_FILE_H
#define RP_FIND_FILE_H

#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

class rp_find_file : public QWidget
{
    Q_OBJECT
public:
    explicit rp_find_file(QWidget *parent = 0);
    ~rp_find_file();

private:
    void rp_findFileWindowUI();
    void rp_findFileFuncConfig();

private:
    QComboBox *rp_createComboBox(const QString &text = QString());
    void rp_updateComboBox(QComboBox *comboBox);

    void rp_createFilesTable();

    void rp_recursionFindFile(const QString &path, const QString &pattern, QStringList *files);
    QStringList rp_findContentFile(const QStringList &files, const QString &text);
    void rp_showFiles(const QStringList &files);

private:
    QComboBox *rp_fileNameComboBox;
    QComboBox *rp_findContentComboBox;
    QComboBox *rp_directoryComboBox;
    QPushButton *rp_browsePushButton;
    QLabel *rp_filesFoundLabel;
    QTableWidget *rp_fileListTable;
    QPushButton *rp_findPushButton;

    QDir rp_currentDir;

private slots:
    void rp_browse();
    void rp_findFile();
    void rp_animateFindClick();
    void rp_openFileOfItem(int row, int column);
    void rp_comtextMenu(const QPoint &pos);
};

#endif // RP_FIND_FILE_H
