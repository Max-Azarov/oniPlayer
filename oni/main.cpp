#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
    w.setGeometry(30, 30, 600, 100);
    w.show();

    return a.exec();
}
