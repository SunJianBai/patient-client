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
    // 性别选项互斥
    ui->radioButton->setAutoExclusive(true);
    ui->radioButton_2->setAutoExclusive(true);
    // 报错文本初始隐藏
    ui->label_3->setText("");
    ui->label_3->setStyleSheet("color: red;");
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
    // 获取性别选项
    QString gender = ui->radioButton->isChecked() ? "男" : (ui->radioButton_2->isChecked() ? "女" : "");
    phone = ui->signPhone->text();
    number = ui->signNumber->text();
    area = ui->signArea->text();
    // 密码一致性检查
    if (password != repassword) {
        ui->label_3->setText("密码与重复密码不一致，请重新输入！");
        return;
    } else {
        ui->label_3->setText("");
    }
    qDebug() << "注册信息:" << username << password << repassword << name << gender << phone << number << area;
    //TODO:


}

SignupForm::~SignupForm()
{
    delete ui;
}
