#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("i13tech");
    QCoreApplication::setOrganizationDomain("i13tech.com");
    QCoreApplication::setApplicationName("Looser");

    MainWindow w;
    w.show();

    return a.exec();
}
