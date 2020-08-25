#ifndef RPLAN_H
#define RPLAN_H

#include <QMainWindow>
#include "rp_cmd/rp_cmd_console.h"
#include "rp_rtt_ota_pack/rp_rtt_ota_pack.h"
#include "rp_find_file/rp_find_file.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTabWidget;
class QSessionManager;
QT_END_NAMESPACE

enum rp_funTabWidgetIndex
{
    rp_cmdTab,
    rp_rttOtaPackTab,
    rp_findFileTab,
};

class rplan : public QMainWindow
{
    Q_OBJECT

public:
    rplan(QWidget *parent = nullptr);
    ~rplan();

private slots:
    void rp_ceShift();
    void rp_help();
    void rp_switchFuncTabWidget();

private:
    void rp_mainWindowUI();
    void rp_createStatusBar();

private:
    QTabWidget *rp_funTabWidget;

    QAction *rp_ceShiftAction;

private:
    rp_cmd_console *rp_cmdWidget;
    rp_rtt_ota_pack *rp_rttOtaPackWidget;
    rp_find_file *rp_findFileWidget;
};
#endif // RPLAN_H
