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

    // 创建错误提示标签，初始隐藏
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setText("");
    errorLabel->setVisible(false);
    // 将 errorLabel 添加到主界面布局
    ui->gridLayout_2->addWidget(errorLabel, 3, 0, 1, 2);


    mainPage = new Main_Page(nullptr);  // init main_page

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
    if (validateLogin(username, password)) {
        qDebug() << "登录成功:" << username << password;
        errorLabel->setVisible(false);
        // TODO: 跳转到主页面（main_page），此处可根据实际需求实现
        mainPage -> show();
        qDebug() << "open main_page";
        this->hide();

    } else {
        errorLabel->setText("账号或密码不正确，请重试。");
        errorLabel->setVisible(true);
        qDebug() << "登录失败:" << username << password;
    }
}


// 简单验证函数，当前总是返回 true，可根据实际需求修改
bool MainWindow::validateLogin(const QString &username, const QString &password)
{
    Q_UNUSED(username);
    Q_UNUSED(password);

    // TODO:
    return true;
}

