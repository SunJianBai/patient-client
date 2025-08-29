#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>
#include "signupform.h"
#include "settingdialog.h"
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sighupBtn_clicked(bool checked);
    void on_loginBtn_clicked(bool checked);
    void on_settingBtn_clicked();

private:
    Ui::MainWindow *ui;
    SignupForm * signPage;
    SettingDialog *m_settingDialog;
    QTcpSocket *m_socket;
    bool validateLogin(const QString &username, const QString &password);
};
#endif // MAINWINDOW_H
