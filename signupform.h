#ifndef SIGNUPFORM_H
#define SIGNUPFORM_H

#include <QDebug>
#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>

namespace Ui {
class SignupForm;
}

class SignupForm : public QWidget
{
    Q_OBJECT
public:
    explicit SignupForm(QWidget *parent = nullptr, QWidget *loginWin = nullptr);
    ~SignupForm();

private slots:
    void on_signBack_clicked();
    void on_signSubmit_clicked();

private:
    Ui::SignupForm *ui;
    QWidget *loginWindow; // 新增成员
    QTcpSocket *m_socket = nullptr; // 新增socket成员
    QString username;
    QString password;
    QString repassword;
    QString name;
    QString gender;
    QString phone;
    QString number;
    QString area;
public:
    void setSocket(QTcpSocket *socket) { m_socket = socket; }
};

#endif // SIGNUPFORM_H
