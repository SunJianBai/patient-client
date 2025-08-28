#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "main_page.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    signPage = new SignupForm(nullptr, this);  // 传入登录窗口指针
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sighupBtn_clicked(bool checked)
{
    signPage->show();
    qDebug() << "open Sign up Page";
    this->hide();
}

bool MainWindow::validateLogin(const QString &username, const QString &password)
{
    Q_UNUSED(username);
    Q_UNUSED(password);
    return true;
}

void MainWindow::on_loginBtn_clicked(bool checked)
{
    QString username = ui->usernm->text();
    QString password = ui->passwd->text();
    qDebug() << "登录信息:" << username << password;
    if (validateLogin(username, password)) {
        Main_Page *mainPage = new Main_Page();
        mainPage->show();
        this->hide();
    }
}
