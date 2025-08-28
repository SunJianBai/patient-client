#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "signupform.h"

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

private:
    Ui::MainWindow *ui;
    SignupForm * signPage;
    bool validateLogin(const QString &username, const QString &password);
};
#endif // MAINWINDOW_H
