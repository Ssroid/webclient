#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSslSocket>
#include <QUdpSocket>
#include <QCheckBox>
#include <QString>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void pbEnabled();
    bool sslStatus;
    QString protocol = "";
    QString filename = "Save_value.txt";

    QTcpSocket socketTcp_;
    QSslSocket socket_;
    // QUdpSocket socket_;
    QCheckBox chkbox_;

public slots:
    void doConnected();
    void doDisconnected();
    void doReadyRead();
    void doErrorOccurred(QAbstractSocket::SocketError socketError);
    void doSaveValue();
    void inputEnabled(bool able);
    void fileRead(QString filename);
    void fileWrite(QString filename);

private slots:
    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

    void on_pbClear_clicked();

    void on_pbSend_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
