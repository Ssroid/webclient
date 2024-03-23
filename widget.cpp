#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    fileRead(filename);
    pbEnabled();

    // socket_.connected();
    // socket_.readyRead();
    QObject::connect(&socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&socket_, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);
    QObject::connect(&socket_, &QAbstractSocket::errorOccurred, this, &Widget::doErrorOccurred);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::pbEnabled() {
    sslStatus = ui->cbSsl->isChecked();
    if(sslStatus) {
        protocol = "SSL";
    }
    else {
        protocol = "TCP";
    }
    ui->pbConnect->setEnabled(socket_.state() == QAbstractSocket::UnconnectedState);
    ui->pbDisconnect->setEnabled(socket_.state() != QAbstractSocket::UnconnectedState);
    ui->pbSend->setEnabled(socket_.state() != QAbstractSocket::UnconnectedState);
}


void Widget::inputEnabled(bool able)
{
    ui->cbSsl->setEnabled(able);
    ui->leHost->setEnabled(able);
    ui->lePort->setEnabled(able);
    sslStatus = ui->cbSsl->isChecked(); //ssl not check, but use 443 port
}


void Widget::fileWrite(QString filename) {
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        ui->pteMessage->insertPlainText("Could not open file for write\n");
        return;
    }
    file.resize(0);
    QTextStream out(&file);
    out << this->x() << "," << this->y() << "\n\n";
    out << ui->leHost->text() << "\n\n";
    out << ui->lePort->text() << "\n\n";
    out << ui->cbSsl->isChecked() << "\n\n";
    out << ui->pteSend->toPlainText();
    out << ui->pteMessage->toPlainText();

    file.flush();
    file.close();
}


void Widget::fileRead(QString filename) {
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        ui->pteMessage->insertPlainText("Could not open file for read\n");
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    QStringList widget_init = text.split("\n\n");

    QStringList pos = widget_init[0].split(",");
    this->move(pos[0].toInt(), pos[1].toInt());

    ui->leHost->setText(widget_init[1]);
    ui->lePort->setText(widget_init[2]);
    if(widget_init[3]=="1")
        ui->cbSsl->setChecked(true);
    ui->pteSend->setPlainText(widget_init[4] + "\r\n\r\n");
    for(int i = 5; i < widget_init.size(); ++i) {
        QString msg = widget_init[i];
        ui->pteMessage->insertPlainText(msg + "\n\n");
    }
    file.close();
}


void Widget::doErrorOccurred(QAbstractSocket::SocketError socketError)
{
    switch(socketError) {
        case QAbstractSocket::HostNotFoundError:
            ui->pteMessage->insertPlainText("Host not found.\n");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            ui->pteMessage->insertPlainText("connection denied.\n");
            break;
        default:
            ui->pteMessage->insertPlainText(socket_.errorString() + "\n");
            break;
    }
    pbEnabled();
    inputEnabled(true);
}


void Widget::doConnected()
{
    pbEnabled();
    ui->pteMessage->insertPlainText(protocol + " Connected \r\n");
}

void Widget::doDisconnected()
{
    pbEnabled();
    ui->pteMessage->insertPlainText(protocol + " Disconected \r\n");
    inputEnabled(true);
}


void Widget::doReadyRead()
{
    ui->pteMessage->insertPlainText(socket_.readAll());
}


void Widget::doSaveValue()
{
    fileWrite(filename);
}


void Widget::on_pbConnect_clicked()
{
    inputEnabled(false);
    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(false);
    ui->pbSend->setEnabled(false);

    if(sslStatus) {
        socket_.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort()); //SSL
    }
    else {
        socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort()); // TCP
    }
}


void Widget::on_pbDisconnect_clicked()
{
    socket_.close();
}


void Widget::on_pbSend_clicked()
{
    socket_.write(ui->pteSend->toPlainText().toUtf8());
}


void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
    pbEnabled();
}


