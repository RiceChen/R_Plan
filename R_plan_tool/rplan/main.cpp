#include "rplan.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    rplan w;
    w.show();
    return a.exec();
}
