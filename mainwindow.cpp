#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main_page.h"
#include "settingdialog.h"
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(new QTcpSocket(this))
    , m_settingDialog(nullptr)
{
    ui->setupUi(this);
    signPage = new SignupForm(nullptr, this);  // 传入登录窗口指针

    // 初始化数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("user.db");
    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", db.lastError().text());
    } else {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS user (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, remember INTEGER)");
    }

    // 自动填充上一次记住的账号密码
    QSqlQuery query;
    query.exec("SELECT username, password FROM user WHERE remember=1 ORDER BY id DESC LIMIT 1");
    if (query.next()) {
        ui->usernm->setText(query.value(0).toString());
        ui->passwd->setText(query.value(1).toString());
    }

    connect(ui->settingBtn, &QToolButton::clicked, this, &MainWindow::on_settingBtn_clicked);
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
    bool remember = ui->rememberCheckBox->isChecked();
    // 检查必填项
    if (username.isEmpty() || password.isEmpty()) {
        ui->loginErrorLabel->setText("用户名和密码不能为空！");
        ui->loginErrorLabel->setStyleSheet("color: red;");
        ui->loginErrorLabel->show();
        return;
    } else {
        ui->loginErrorLabel->setText("");
        ui->loginErrorLabel->hide();
    }
    qDebug() << "登录信息:" << username << password;
    if (validateLogin(username, password)) {
        // 保存到数据库
        QSqlQuery query;
        if (remember) {
            query.exec("DELETE FROM user WHERE remember=1");
            query.prepare("INSERT INTO user (username, password, remember) VALUES (?, ?, 1)");
            query.addBindValue(username);
            query.addBindValue(password);
            query.exec();
        }
        Main_Page *mainPage = new Main_Page();
        mainPage->show();
        this->hide();
    }
}

void MainWindow::on_settingBtn_clicked()
{
    if (!m_settingDialog)
        m_settingDialog = new SettingDialog(m_socket, this);
    m_settingDialog->show();
    m_settingDialog->raise();
    m_settingDialog->activateWindow();
}
