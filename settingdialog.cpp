#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>

SettingDialog::SettingDialog(QTcpSocket *socket, QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingDialog), m_socket(socket)
{
    ui->setupUi(this);
    connect(ui->connectBtn, &QPushButton::clicked, this, &SettingDialog::on_connectBtn_clicked);
    connect(ui->disconnectBtn, &QPushButton::clicked, this, &SettingDialog::on_disconnectBtn_clicked);
    connect(m_socket, &QTcpSocket::stateChanged, this, &SettingDialog::onSocketStateChanged);
    onSocketStateChanged(m_socket->state());
}

SettingDialog::~SettingDialog() {
    delete ui;
}

void SettingDialog::on_connectBtn_clicked() {
    QString ip = ui->ipEdit->text();
    quint16 port = ui->portEdit->text().toUShort();
    m_socket->connectToHost(QHostAddress(ip), port);
    // 连接成功后自动发送一条 JSON（带长度前缀）
    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        QJsonObject obj;
        obj["type"] = "client_hello";
        obj["msg"] = "hello server";
        QJsonDocument doc(obj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        m_socket->write(packet);
        m_socket->flush();
    });
}

void SettingDialog::on_disconnectBtn_clicked() {
    m_socket->disconnectFromHost();
}

void SettingDialog::onSocketStateChanged(QAbstractSocket::SocketState state) {
    QString status;
    switch (state) {
    case QAbstractSocket::ConnectedState:
        status = "已连接";
        break;
    case QAbstractSocket::ConnectingState:
        status = "连接中...";
        break;
    case QAbstractSocket::UnconnectedState:
        status = "未连接";
        break;
    default:
        status = "状态: " + QString::number(state);
        break;
    }
    ui->statusLabel->setText(status);
}
