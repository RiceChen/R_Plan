#ifndef RPLAN_H
#define RPLAN_H

#include <QMainWindow>
#include "rp_cmd/rp_cmd_console.h"
#include "rp_rtt_ota_pack/rp_rtt_ota_pack.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTabWidget;
class QSessionManager;
QT_END_NAMESPACE

class rplan : public QMainWindow
{
    Q_OBJECT

public:
    rplan(QWidget *parent = nullptr);
    ~rplan();

private slots:
    void rp_help();

private:
    void rp_mainWindowUI();
    void rp_createStatusBar();

private:
    QTabWidget *rp_funTabWidget;

private:
    rp_cmd_console *rp_cmdWidget;
    rp_rtt_ota_pack *rp_rttOtaPackWidget;


};
#endif // RPLAN_H
