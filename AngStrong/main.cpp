#include "angstrong.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AngStrong w;
    w.show();
    return a.exec();
}
