#include "signupform.h"
#include "ui_signupform.h"
#include <QDebug>

SignupForm::SignupForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignupForm)
{
    ui->setupUi(this);
    connect(ui->signBack, &QPushButton::clicked, this, &SignupForm::on_signBack_clicked);
    connect(ui->signSubmit, &QPushButton::clicked, this, &SignupForm::on_signSubmit_clicked);
}
void SignupForm::on_signBack_clicked()
{
    this->hide();
    if (parentWidget()) parentWidget()->show();
}

void SignupForm::on_signSubmit_clicked()
{
    QString username = ui->signusername->text();
    QString password = ui->signpasswd->text();
    QString repassword = ui->reppasswd->text();
    QString name = ui->signName->text();
    QString gender = ui->signGender->text();
    QString phone = ui->signPhone->text();
    QString number = ui->signNumber->text();
    QString area = ui->signArea->text();
    qDebug() << "注册信息:" << username << password << repassword << name << gender << phone << number << area;

    //TODO:
}

SignupForm::~SignupForm()
{
    delete ui;
}
