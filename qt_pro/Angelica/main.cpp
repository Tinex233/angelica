#include "loginWidget.h"

#include <QApplication>
#include <QSslSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //生成配置文件路径（QSetting）
    QCoreApplication::setOrganizationName("MyCompany");
    QCoreApplication::setApplicationName("Angelica+");

    loginWidget l;
    l.show();

    return a.exec();
}
