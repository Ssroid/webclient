#include "widget.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    // Save the value of widget when the widget is quit
    QObject::connect(&a, &QCoreApplication::aboutToQuit, &w, &Widget::doSaveValue);
    return a.exec();
}
