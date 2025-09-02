#include "signupform.h"
#include "ui_signupform.h"

SignupForm::~SignupForm()
{
    delete ui;
}


SignupForm::SignupForm(QWidget *parent, QWidget *loginWin) :
    QWidget(parent),
    ui(new Ui::SignupForm),
    loginWindow(loginWin)
{
    ui->setupUi(this);
    // 设置标题
    ui->emptyLabel->setText("用户注册");
    ui->emptyLabel->setAlignment(Qt::AlignCenter);
    // 设置注册界面样式
    ui->signError->hide();
    this->setStyleSheet(R"(
        /* 注册窗口整体样式 */
        SignupForm {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                stop: 0 #f5f7fa, stop: 1 #c3cfe2);
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }

        /* 标题标签样式 */
        QLabel#emptyLabel {
            font-size: 24px;
            font-weight: bold;
            color: #2c3e50;
            padding: 15px 0;
            background: rgba(255, 255, 255, 0.7);
            border-radius: 8px;
            margin-bottom: 15px;
            border: 1px solid #bdc3c7;
        }

        /* 主布局容器 */
        QVBoxLayout#verticalLayout {
            margin: 30px;
            spacing: 15px;
            background-color: rgba(255, 255, 255, 0.9);
            border-radius: 12px;
            padding: 25px;
            border: 1px solid #bdc3c7;
        }

        /* 标签样式 */
        QLabel {
            color: #2c3e50;
            font-size: 13px;
            font-weight: bold;
            padding: 5px 0;
        }

        /* 空标签隐藏 */
        QLabel#emptyLabel {
            background: transparent;
            border: none;
            padding: 0;
            margin: 0;
        }

        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
            color: #e74c3c;
            font-size: 12px;
            padding: 8px 12px;
            background-color: #fadbd8;
            border-radius: 6px;
            border: 1px solid #f5b7b1;
            margin: 5px 0;
            qproperty-alignment: AlignCenter;
        }

        QLabel#signError[text=""] {
            background: transparent;
            border: none;
            padding: 0;
            margin: 0;
            height: 0;
        }

        /* 输入框样式 */
        QLineEdit {
            background: white;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            padding: 8px 12px;
            font-size: 13px;
            color: #2c3e50;
            min-height: 20px;
        }

        QLineEdit:focus {
            border: 1px solid #3498db;
        }

        QLineEdit:hover {
            border: 1px solid #95a5a6;
        }

        /* 单选按钮样式 */
        QRadioButton {
            color: #2c3e50;
            font-size: 13px;
            spacing: 8px;
            padding: 5px 0;
        }

        QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #bdc3c7;
            border-radius: 8px;
            background: white;
        }

        QRadioButton::indicator:checked {
            background-color: #3498db;
            border: 1px solid #3498db;
        }

        QRadioButton::indicator:hover {
            border: 1px solid #95a5a6;
        }

        /* 按钮样式 */
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #2980b9;
        }

        QPushButton:pressed {
            background-color: #21618c;
        }

        /* 返回按钮特殊样式 */
        QPushButton#signBack {
            background-color: #95a5a6;
        }

        QPushButton#signBack:hover {
            background-color: #7f8c8d;
        }

        QPushButton#signBack:pressed {
            background-color: #6c7a7b;
        }

        /* 注册按钮特殊样式 */
        QPushButton#signSubmit {
            background-color: #2ecc71;
        }

        QPushButton#signSubmit:hover {
            background-color: #27ae60;
        }

        QPushButton#signSubmit:pressed {
            background-color: #219653;
        }

        /* 布局样式 */
        QFormLayout, QGridLayout, QHBoxLayout {
            margin: 5px;
            spacing: 10px;
        }

        QHBoxLayout#horizontalLayout_2 {
            spacing: 15px;
        }
    )");
    // 防止注册按钮重复连接槽函数
    disconnect(ui->signSubmit, nullptr, nullptr, nullptr);
    connect(ui->signBack, &QPushButton::clicked, this, &SignupForm::on_signBack_clicked);
    connect(ui->signSubmit, &QPushButton::clicked, this, &SignupForm::on_signSubmit_clicked);
    // 性别选项互斥
    ui->radioButton->setAutoExclusive(true);
    ui->radioButton_2->setAutoExclusive(true);
    // 报错文本初始隐藏
    ui->signError->setText("");
    ui->signError->setStyleSheet("color: red;");
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
    // 检查必填项
    if (username.isEmpty() || password.isEmpty() || repassword.isEmpty() || name.isEmpty() || gender.isEmpty() || phone.isEmpty() || number.isEmpty() || area.isEmpty()) {
        ui->signError->setText("所有选项均为必填，请完整填写！");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
        ui->signError->show();
        return;
    }
    // 密码一致性检查
    else if (password != repassword) {
        ui->signError->setText("密码与重复密码不一致，请重新输入！");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        return;
    }
    else {
        ui->signError->setText("");
    }
    qDebug() << "注册信息:" << username << password << repassword << name << gender << phone << number << area;

    // 检查socket
    qDebug() << "[注册] socket实例:" << m_socket;
    if (!m_socket) {
        ui->signError->setText("服务器出错");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        qDebug() << "[注册] socket为空";
        return;
    }
    qDebug() << "[注册] socket状态:" << m_socket->state();
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        ui->signError->setText("服务器出错");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        qDebug() << "[注册] socket未连接";
        return;
    }
    // 构造注册JSON
    QJsonObject payload;
    payload["user"] = username;
    payload["passwd"] = password;
    payload["role"] = "patient";
    payload["name"] = name;
    payload["gender"] = gender;
    payload["phone"] = phone;
    payload["id_number"] = number;
    payload["adress"] = area;
    QJsonObject obj;
    obj["type"] = "register";
    obj["seq"] = 1002;
    obj["payload"] = payload;
    QJsonDocument doc(obj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    qDebug() << "[注册] 发送json:" << QString::fromUtf8(jsonData);
    // 发送数据（带长度前缀）
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream << static_cast<quint32>(jsonData.size());
    packet.append(jsonData);
    qDebug() << "[注册] 发送packet长度:" << packet.size();
    m_socket->write(packet);
    m_socket->flush();

    // 阻塞等待服务器返回
    if (!m_socket->waitForReadyRead(3000)) {
        ui->signError->setText("服务器出错");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        qDebug() << "[注册] 服务器无返回";
        return;
    }
    QByteArray resp = m_socket->readAll();
    qDebug() << "[注册] 收到resp长度:" << resp.size();
    if (resp.size() < 4) {
        ui->signError->setText("服务器出错");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        qDebug() << "[注册] 返回数据长度异常";
        return;
    }
    QDataStream respStream(resp);
    respStream.setVersion(QDataStream::Qt_5_10);
    quint32 len = 0;
    respStream >> len;
    QByteArray jsonResp = resp.right(resp.size() - 4);
    qDebug() << "[注册] 返回json:" << QString::fromUtf8(jsonResp);
    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
    if (!respDoc.isObject()) {
        ui->signError->setText("服务器出错");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
         ui->signError->show();
        qDebug() << "[注册] 返回json解析失败";
        return;
    }
    QJsonObject respObj = respDoc.object();
    if (respObj.value("seq").toInt() == 1002 && respObj.value("ok").toBool()) {
        QMessageBox::information(this, "注册成功", "注册成功！");
        qDebug() << "[注册] 注册成功";
        this->hide();
        if (loginWindow) loginWindow->show();
    } else {
        ui->signError->setText("该用户名已被使用，请更换");
        ui->signError->setStyleSheet(R"(
        /* 错误提示标签 - 初始隐藏 */
        QLabel#signError {
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
        ui->signError->show();
        qDebug() << "[注册] 注册失败";
        return;
    }
}
