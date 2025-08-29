#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QTcpSocket *socket, QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingDialog), m_socket(socket)
{
    ui->setupUi(this);
    connect(ui->connectBtn, &QPushButton::clicked, this, &SettingDialog::on_connectBtn_clicked);
    connect(ui->disconnectBtn, &QPushButton::clicked, this, &SettingDialog::on_disconnectBtn_clicked);
    connect(m_socket, &QTcpSocket::stateChanged, this, &SettingDialog::onSocketStateChanged);
    onSocketStateChanged(m_socket->state());

    // 自动填充上一次连接的IP和端口
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "setting_conn");
    db.setDatabaseName("user.db");
    if (db.open()) {
        QSqlQuery query(db);
        query.exec("CREATE TABLE IF NOT EXISTS setting (id INTEGER PRIMARY KEY AUTOINCREMENT, ip TEXT, port INTEGER)");
        query.exec("SELECT ip, port FROM setting ORDER BY id DESC LIMIT 1");
        if (query.next()) {
            ui->ipEdit->setText(query.value(0).toString());
            ui->portEdit->setText(query.value(1).toString());
        }
    }
}

SettingDialog::~SettingDialog() {
    delete ui;
}

void SettingDialog::on_connectBtn_clicked() {
    QString ip = ui->ipEdit->text();
    quint16 port = ui->portEdit->text().toUShort();
    m_socket->connectToHost(QHostAddress(ip), port);
    // 连接成功后自动发送一条 JSON（带长度前缀）
    connect(m_socket, &QTcpSocket::connected, this, [this, ip, port]() {
        // 持久化IP和端口
        QSqlDatabase db = QSqlDatabase::database("setting_conn");
        if (db.isOpen()) {
            QSqlQuery query(db);
            query.exec("DELETE FROM setting");
            query.prepare("INSERT INTO setting (ip, port) VALUES (?, ?)");
            query.addBindValue(ip);
            query.addBindValue(port);
            query.exec();
        }
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
