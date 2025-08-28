#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    signPage = new SignupForm(nullptr);  // init sign up page

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
