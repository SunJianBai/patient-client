#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H
#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingDialog(QTcpSocket *socket, QWidget *parent = nullptr);
    ~SettingDialog();
private slots:
    void on_connectBtn_clicked();
    void on_disconnectBtn_clicked();
    void onSocketStateChanged(QAbstractSocket::SocketState state);
private:
    Ui::SettingDialog *ui;
    QTcpSocket *m_socket;
};
#endif // SETTINGDIALOG_H
