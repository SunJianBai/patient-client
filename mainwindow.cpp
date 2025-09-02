#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include "usercontext.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_socket(nullptr)
    , m_settingDialog(nullptr)
{
    ui->setupUi(this);
    //设置样式
    ui->loginErrorLabel->hide();
    this->setStyleSheet(R"(
        /* 主窗口样式 */
        MainWindow {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                stop: 0 #f5f7fa, stop: 1 #c3cfe2);
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }

        /* 中央部件样式 */
        QVBoxLayout#verticalLayout {
            background: rgba(255, 255, 255, 0.9);
            border-radius: 0.84em;
            margin: 2.8em;
            padding: 1.75em;
            border: 0.07em solid #bdc3c7;
        }

        /* 标题标签样式 */
        QLabel#titleLabel {
            font-size: 18pt;
            font-weight: bold;
            color: #2c3e50;
            padding: 1.05em 0;
            background: rgba(255, 255, 255, 0.7);
            border-radius: 0.56em;
            margin-bottom: 1.4em;
            border: 0.07em solid #bdc3c7;
        }

        /* 表单标签样式 */
        QLabel {
            color: #2c3e50;
            font-size: 9.5pt;
            font-weight: bold;
            padding: 0.35em 0;
        }

        /* 错误提示标签 */
        QLabel#loginErrorLabel {
            color: #e74c3c;
            font-size: 9pt;
            padding: 0.56em 0.84em;
            background-color: #fadbd8;
            border-radius: 0.42em;
            border: 0.07em solid #f5b7b1;
            margin: 0.35em 0;
            qproperty-alignment: AlignCenter;
        }

        QLabel#loginErrorLabel[text=""] {
            background: transparent;
            border: none;
            padding: 0;
            margin: 0;
            height: 0;
        }

        /* 输入框样式 */
        QLineEdit {
            background: white;
            border: 0.07em solid #bdc3c7;
            border-radius: 0.35em;
            padding: 0.56em 0.84em;
            font-size: 9.5pt;
            color: #2c3e50;
            min-height: 1.4em;
        }

        QLineEdit:focus {
            border: 0.07em solid #3498db;
        }

        QLineEdit:hover {
            border: 0.07em solid #95a5a6;
        }

        /* 按钮样式 */
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 0.42em;
            padding: 0.7em 1.4em;
            font-size: 10.5pt;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #2980b9;
        }

        QPushButton:pressed {
            background-color: #21618c;
        }

        /* 注册按钮特殊样式 */
        QPushButton#sighupBtn {
            background-color: #2ecc71;
        }

        QPushButton#sighupBtn:hover {
            background-color: #27ae60;
        }

        QPushButton#sighupBtn:pressed {
            background-color: #219653;
        }

        /* 设置按钮样式 */
        QToolButton#settingBtn {
            background: #95a5a6;
            color: white;
            border: none;
            border-radius: 0.42em;
            padding: 0.56em 0.84em;
            font-size: 12pt;
            font-weight: bold;
        }

        QToolButton#settingBtn:hover {
            background: #7f8c8d;
        }

        QToolButton#settingBtn:pressed {
            background: #6c7a7b;
        }

        /* 复选框样式 */
        QCheckBox {
            color: #2c3e50;
            font-size: 9.5pt;
            spacing: 0.56em;
            padding: 0.35em 0;
        }

        QCheckBox::indicator {
            width: 1.12em;
            height: 1.12em;
            border: 0.07em solid #bdc3c7;
            border-radius: 0.28em;
            background: white;
        }

        QCheckBox::indicator:checked {
            background: #3498db;
            border: 0.07em solid #3498db;
        }

        QCheckBox::indicator:hover {
            border: 0.07em solid #95a5a6;
        }

        /* 布局边距调整 */
        QGridLayout#loginFormLayout, QVBoxLayout, QFormLayout {
            margin: 0.35em;
            spacing: 0.7em;
        }

        /* 表单布局标签对齐 */
        QFormLayout::label {
            padding-right: 1.05em;
        }
    )");

    mainPage = new Main_Page();
    mainPage->setSocket(m_socket); // 传递socket实例
    signPage = new SignupForm(nullptr, this);  // 传入登录窗口指针
    signPage->setSocket(m_socket); // 传递socket实例

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
    // 检查连接状态
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "网络错误", "未连接到服务器，请检查设置！");
        return;
    }
    signPage->show();
    qDebug() << "open Sign up Page";
    this->hide();
}

bool MainWindow::validateLogin(const QString &username, const QString &password)
{
    // TEST==================================================================
//    int user_id = 1001;
//    UserContext::instance()->setUserId(user_id);
//    qDebug() << "登录成功，user_id:" << user_id;
//    return true;
    // ======================================================================

    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return false;

    // 构造登录JSON
    QJsonObject obj;
    obj["type"] = "login";
    obj["seq"] = 1001;
    obj["user"] = username;
    obj["pswd"] = password;
    obj["role"] = "patient";
    QJsonDocument doc(obj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    // 发送数据（带长度前缀）
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream << static_cast<quint32>(jsonData.size());
    packet.append(jsonData);
    m_socket->write(packet);
    m_socket->flush();

    // 阻塞等待服务器返回
    if (!m_socket->waitForReadyRead(3000)) // 最多等待3秒
        return false;
    QByteArray resp = m_socket->readAll();
    // 解析长度前缀和JSON
    if (resp.size() < 4) return false;
    QDataStream respStream(resp);
    respStream.setVersion(QDataStream::Qt_5_10);
    quint32 len = 0;
    respStream >> len;
    QByteArray jsonResp = resp.right(resp.size() - 4);
    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
    if (!respDoc.isObject()) return false;
    QJsonObject respObj = respDoc.object();
    if (respObj.value("seq").toInt() == 1001 && respObj.value("ok").toBool()) {
        // 可在此处获取 user_id
        if (respObj.contains("payload")) {
            QJsonObject payload = respObj.value("payload").toObject();
            int user_id = 1001;
            user_id = payload.value("user_id").toInt();
            UserContext::instance()->setUserId(user_id);
            qDebug() << "登录成功，user_id:" << user_id;
        }
        return true;
    } else {
        ui->loginErrorLabel->setText("账号或密码错误");
        ui->loginErrorLabel->setStyleSheet(R"(
        /* 错误提示标签 */
        QLabel#loginErrorLabel {
            color: #e74c3c;
            font-size: 12px;
            padding: 8px 12px;
            background-color: #fadbd8;
            border-radius: 6px;
            border: 1px solid #f5b7b1;
            margin: 5px 0;
            qproperty-alignment: AlignCenter;
        }
)");
        ui->loginErrorLabel->setStyleSheet("color: red;");
        ui->loginErrorLabel->show();
        return false;
    }
}

void MainWindow::on_loginBtn_clicked(bool checked)
{
    QString username = ui->usernm->text();
    QString password = ui->passwd->text();
    bool remember = ui->rememberCheckBox->isChecked();
    // 检查连接状态
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "网络错误", "未连接到服务器，请检查设置！");
        return;
    }
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
    qDebug() << "登录信息:" << username << "|" << password;
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
        mainPage->setSocket(m_socket); // 登录成功后再传递socket，保证user_id已设置
        mainPage->show();
        this->hide();
    }
}

void MainWindow::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    if (signPage) {
        signPage->setSocket(socket);
    }
    // 检查连接状态，显示超时文本
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        QLabel *statusLabel = this->findChild<QLabel*>("statusLabel");
        if (statusLabel) {
            statusLabel->setText("连接超时");
            statusLabel->setStyleSheet("color: red;");
        }
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
