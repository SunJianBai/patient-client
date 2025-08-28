#include "signupform.h"
#include "ui_signupform.h"
#include <QDebug>

SignupForm::SignupForm(QWidget *parent, QWidget *loginWin) :
    QWidget(parent),
    ui(new Ui::SignupForm),
    loginWindow(loginWin)
{
    ui->setupUi(this);
    connect(ui->signBack, &QPushButton::clicked, this, &SignupForm::on_signBack_clicked);
    connect(ui->signSubmit, &QPushButton::clicked, this, &SignupForm::on_signSubmit_clicked);
}

void SignupForm::on_signBack_clicked()
{
    this->hide();
    if (loginWindow) loginWindow->show();
}

void SignupForm::on_signSubmit_clicked()
{
    username = ui->signusername->text();
    password = ui->signpasswd->text();
    repassword = ui->reppasswd->text();
    name = ui->signName->text();
    gender = ui->signGender->text();
    phone = ui->signPhone->text();
    number = ui->signNumber->text();
    area = ui->signArea->text();
    qDebug() << "注册信息:" << username << password << repassword << name << gender << phone << number << area;
    //TODO:
}

SignupForm::~SignupForm()
{
    delete ui;
}
