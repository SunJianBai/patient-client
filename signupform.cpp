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
        return;
    }
    // 密码一致性检查
    else if (password != repassword) {
        ui->signError->setText("密码与重复密码不一致，请重新输入！");
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
        qDebug() << "[注册] socket为空";
        return;
    }
    qDebug() << "[注册] socket状态:" << m_socket->state();
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        ui->signError->setText("服务器出错");
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
        qDebug() << "[注册] 服务器无返回";
        return;
    }
    QByteArray resp = m_socket->readAll();
    qDebug() << "[注册] 收到resp长度:" << resp.size();
    if (resp.size() < 4) {
        ui->signError->setText("服务器出错");
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
        qDebug() << "[注册] 注册失败";
        return;
    }
}
