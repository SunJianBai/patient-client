#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "signupform.h"
#include "main_page.h"

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
    void on_loginBtn_clicked();

private:
    Ui::MainWindow *ui;
    SignupForm * signPage;
    Main_Page * mainPage;

    QLabel *errorLabel; // 登录错误提示
    bool validateLogin(const QString &username, const QString &password);
};
#endif // MAINWINDOW_H
