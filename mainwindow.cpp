#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    signPage = new SignupForm(nullptr);  // init sign up page

        connect(ui->loginBtn, &QPushButton::clicked, this, &MainWindow::on_loginBtn_clicked);

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

void MainWindow::on_loginBtn_clicked()
{
    QString username = ui->usernm->text();
    QString password = ui->passwd->text();
    qDebug() << "登录信息:" << username << password;
}
