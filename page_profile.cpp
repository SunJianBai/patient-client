#include "main_page.h"
#include "page_profile.h"
#include "ui_page_profile.h"


PageProfile::PageProfile(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Profile) {
    ui->setupUi(this);
    QString styleSheet=R"(
        /* 主窗口样式 */
        Page_Profile {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                stop: 0 #f5f7fa, stop: 1 #c3cfe2);
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }

        /* 面板容器样式 */
        QFrame, QWidget {
            background: rgba(255, 255, 255, 0.9);
            border: 0.07em solid #bdc3c7;
            border-radius: 0.56em;
            padding: 1.05em;
        }

        /* 标题标签样式 */
        QLabel {
            color: #2c3e50;
            font-size: 9.5pt;
        }

        QLabel[label_10], QLabel[label_6], QLabel[label] {
            font-size: 12pt;
            font-weight: bold;
            color: #3498db;
            padding-bottom: 0.7em;
        }

        /* 数据显示标签样式 */
        #name, #gender, #phone, #id, #adress {
            background: #f8f9fa;
            border: 0.07em solid #e9ecef;
            border-radius: 0.35em;
            padding: 0.56em 0.84em;
            color: #2c3e50;
            font-size: 9.5pt;
            min-height: 1.4em;
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

        /* 按钮样式 */
        QPushButton {
            background: #3498db;
            color: white;
            border: none;
            border-radius: 0.42em;
            padding: 0.7em 1.4em;
            font-weight: bold;
            font-size: 10.5pt;
        }

        QPushButton:hover {
            background: #2980b9;
        }

        QPushButton:pressed {
            background: #21618c;
        }

        /* 返回按钮样式 */
        #back1, #back2 {
            background: #95a5a6;
        }

        #back1:hover, #back2:hover {
            background: #7f8c8d;
        }

        /* 错误提示文本 */
        #errortext2, #label_16 {
            color: #e74c3c;
            font-size: 9pt;
            padding: 0.35em 0;
        }

        /* 布局间距调整 */
        QGridLayout, QVBoxLayout, QHBoxLayout {
            margin: 0.35em;
            spacing: 0.7em;
        }

        /* 表单标签对齐 */
        QLabel[label_7], QLabel[label_8], QLabel[label_9],
        QLabel[label_11], QLabel[label_12], QLabel[label_13],
        QLabel[label_14], QLabel[label_15],
        QLabel[label_2], QLabel[label_3], QLabel[label_4], QLabel[label_5] {
            font-weight: bold;
            min-width: 5.6em;
        }
)";
    this->setStyleSheet(styleSheet);
    // 优化初始显示：只显示个人信息板块
    ui->info->setVisible(true);
    ui->cg_info->setVisible(false);
    ui->cg_passwd->setVisible(false);
    // 初始显示时刷新
    refreshUserInfo();

}


void PageProfile::refreshUserInfo() {
    QTcpSocket *socket = nullptr;
    QWidget *p = parentWidget();
    while (p) {
        Main_Page *mainPage = qobject_cast<Main_Page *>(p);
        if (mainPage) {
            socket = mainPage->m_socket;
            break;
        }
        p = p->parentWidget();
    }
    qDebug() << "[Profile] refreshUserInfo() 获取到socket:" << socket;
    int user_id = UserContext::instance()->userId();
    qDebug() << "[Profile] 刷新个人信息，user_id=" << user_id;
    if (socket && socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
        QJsonObject req;
        req["type"] = "userinfo";
        req["seq"] = 1099;
        req["user_id"] = user_id;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Profile] 发送个人信息请求:" << QString::fromUtf8(jsonData);
        socket->write(packet);
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QByteArray resp = socket->readAll();
            qDebug() << "[Profile] 收到个人信息反馈长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Profile] 个人信息返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "userinfo" && respObj.value("seq").toInt() == 1099) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        UserContext::instance()->setFromJson(payload);
                        qDebug() << "[Profile] 更新UserContext:" << payload;
                        // 刷新UI显示
                        ui->name->setText(UserContext::instance()->name());
                        ui->gender->setText(UserContext::instance()->gender());
                        ui->phone->setText(UserContext::instance()->phone());
                        ui->id->setText(UserContext::instance()->idNumber());
                        ui->adress->setText(UserContext::instance()->adress());
                    }
                }
            }
        } else {
            qDebug() << "[Profile] 个人信息服务器无返回";
        }
    } else {
        qDebug() << "[Profile] socket未连接或user_id无效，无法刷新个人信息";
    }


    // 点击“修改个人信息”按钮，切换到修改个人信息板块并自动填充输入框
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=](){
        ui->info->setVisible(false);
        ui->cg_info->setVisible(true);
        ui->cg_passwd->setVisible(false);
        // 自动填充输入框
        ui->cg_name->setText(UserContext::instance()->name());
        ui->cg_phone->setText(UserContext::instance()->phone());
        ui->cg_id->setText(UserContext::instance()->idNumber());
        ui->lineEdit_4->setText(UserContext::instance()->adress());
        qDebug() << "[Profile] 自动填充修改个人信息输入框:" << UserContext::instance()->name() << UserContext::instance()->phone() << UserContext::instance()->idNumber() << UserContext::instance()->adress();
        ui->errortext2->clear();
    });

    // 点击“修改密码”按钮，切换到修改密码板块
    connect(ui->pushButton_4, &QPushButton::clicked, this, [=](){
        ui->info->setVisible(false);
        ui->cg_info->setVisible(false);
        ui->cg_passwd->setVisible(true);
    });


    // 修改个人信息板块的“返回”按钮
    connect(ui->back2, &QPushButton::clicked, this, [=](){
        ui->info->setVisible(true);
        ui->cg_info->setVisible(false);
        ui->cg_passwd->setVisible(false);
        ui->errortext2->clear();
        refreshUserInfo();
    });
    // 修改个人信息板块的“修改”按钮
    connect(ui->pushButton, &QPushButton::clicked, this, [=](){
        QString name = ui->cg_name->text().trimmed();
        QString phone = ui->cg_phone->text().trimmed();
        QString id_number = ui->cg_id->text().trimmed();
        QString adress = ui->lineEdit_4->text().trimmed();
        int user_id = UserContext::instance()->userId();
        qDebug() << "[Profile] 修改个人信息输入:" << name << phone << id_number << adress << "user_id=" << user_id;
        // 本地完整性校验
        if (name.isEmpty() || phone.isEmpty() || id_number.isEmpty() || adress.isEmpty()) {
            QString err = "所有字段均不能为空！";
            ui->errortext2->setText(err);
            ui->errortext2->setStyleSheet("color: red;");
            qDebug() << "[Profile] 校验失败:" << err;
            return;
        }
        ui->errortext2->clear();
        // 构造JSON请求
        QJsonObject payload;
        payload["user_id"] = user_id;
        payload["name"] = name;
        payload["phone"] = phone;
        payload["id_number"] = id_number;
        payload["adress"] = adress;
        QJsonObject req;
        req["type"] = "change_user_info";
        req["seq"] = 1050;
        req["payload"] = payload;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Profile] 发送修改个人信息请求:" << QString::fromUtf8(jsonData);
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(packet);
            socket->flush();
            if (socket->waitForReadyRead(3000)) {
                QByteArray resp = socket->readAll();
                qDebug() << "[Profile] 收到修改个人信息反馈长度:" << resp.size();
                if (resp.size() >= 4) {
                    QDataStream respStream(resp);
                    respStream.setVersion(QDataStream::Qt_5_10);
                    quint32 len = 0;
                    respStream >> len;
                    QByteArray jsonResp = resp.right(resp.size() - 4);
                    qDebug() << "[Profile] 修改个人信息返回json:" << QString::fromUtf8(jsonResp);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                    if (respDoc.isObject()) {
                        QJsonObject respObj = respDoc.object();
                        if (respObj.value("type").toString() == "change_user_info" && respObj.value("seq").toInt() == 1050) {
                            bool ok = respObj.value("ok").toBool();
                            qDebug() << "[Profile] 修改个人信息结果ok:" << ok;
                            if (ok) {
                                QMessageBox::information(this, "修改成功", "个人信息修改成功！");
                                ui->info->setVisible(true);
                                ui->cg_info->setVisible(false);
                                ui->cg_passwd->setVisible(false);
                                refreshUserInfo();
                            } else {
                                QString err = "服务器修改失败！";
                                ui->errortext2->setText(err);
                                ui->errortext2->setStyleSheet("color: red;");
                                qDebug() << "[Profile] 服务器返回失败:" << err;
                            }
                        }
                    }
                }
            } else {
                QString err = "服务器无响应，修改失败！";
                ui->errortext2->setText(err);
                ui->errortext2->setStyleSheet("color: red;");
                qDebug() << "[Profile] 服务器无响应:" << err;
            }
        } else {
            QString err = "未连接到服务器，无法修改！";
            ui->errortext2->setText(err);
            ui->errortext2->setStyleSheet("color: red;");
            qDebug() << "[Profile] socket未连接:" << err;
        }
    });


    // 修改密码板块的“返回”按钮
    connect(ui->back1, &QPushButton::clicked, this, [=](){
        ui->info->setVisible(true);
        ui->cg_info->setVisible(false);
        ui->cg_passwd->setVisible(false);
        if (QLabel *err = ui->cg_passwd->findChild<QLabel*>("label_16")) err->clear();
        refreshUserInfo();
    });

    // 修改密码板块的“修改”按钮
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=](){
        QString old_passwd = ui->lineEdit->text();
        QString new_passwd = ui->lineEdit_2->text();
        QString new_passwd2 = ui->lineEdit_3->text();
        int user_id = UserContext::instance()->userId();
        QLabel *errLabel = ui->cg_passwd->findChild<QLabel*>("label_16");
        qDebug() << "[Profile] 修改密码输入:" << old_passwd << new_passwd << new_passwd2 << "user_id=" << user_id;
        // 本地完整性校验
        if (old_passwd.isEmpty() || new_passwd.isEmpty() || new_passwd2.isEmpty()) {
            QString err = "所有字段均不能为空！";
            if (errLabel) { errLabel->setText(err); errLabel->setStyleSheet("color: red;"); }
            qDebug() << "[Profile] 修改密码校验失败:" << err;
            return;
        }
        if (new_passwd != new_passwd2) {
            QString err = "两次密码应该一致";
            if (errLabel) { errLabel->setText(err); errLabel->setStyleSheet("color: red;"); }
            qDebug() << "[Profile] 修改密码校验失败:" << err;
            return;
        }
        if (errLabel) errLabel->clear();
        // 构造JSON请求
        QJsonObject payload;
        payload["user_id"] = user_id;
        payload["passwd"] = old_passwd;
        payload["new_passwd"] = new_passwd;
        QJsonObject req;
        req["type"] = "change_passwd";
        req["seq"] = 1051;
        req["payload"] = payload;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Profile] 发送修改密码请求:" << QString::fromUtf8(jsonData);
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(packet);
            socket->flush();
            if (socket->waitForReadyRead(3000)) {
                QByteArray resp = socket->readAll();
                qDebug() << "[Profile] 收到修改密码反馈长度:" << resp.size();
                if (resp.size() >= 4) {
                    QDataStream respStream(resp);
                    respStream.setVersion(QDataStream::Qt_5_10);
                    quint32 len = 0;
                    respStream >> len;
                    QByteArray jsonResp = resp.right(resp.size() - 4);
                    qDebug() << "[Profile] 修改密码返回json:" << QString::fromUtf8(jsonResp);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                    if (respDoc.isObject()) {
                        QJsonObject respObj = respDoc.object();
                        if (respObj.value("type").toString() == "change_passwd" && respObj.value("seq").toInt() == 1051) {
                            bool ok = respObj.value("ok").toBool();
                            qDebug() << "[Profile] 修改密码结果ok:" << ok;
                            if (ok) {
                                QMessageBox::information(this, "修改成功", "密码修改成功！");
                                ui->info->setVisible(true);
                                ui->cg_info->setVisible(false);
                                ui->cg_passwd->setVisible(false);
                                if (errLabel) errLabel->clear();
                                refreshUserInfo();
                            } else {
                                QString err = "原密码不正确";
                                if (errLabel) { errLabel->setText(err); errLabel->setStyleSheet("color: red;"); }
                                qDebug() << "[Profile] 服务器返回失败:" << err;
                            }
                        }
                    }
                }
            } else {
                QString err = "服务器无响应，修改失败！";
                if (errLabel) { errLabel->setText(err); errLabel->setStyleSheet("color: red;"); }
                qDebug() << "[Profile] 服务器无响应:" << err;
            }
        } else {
            QString err = "未连接到服务器，无法修改！";
            if (errLabel) { errLabel->setText(err); errLabel->setStyleSheet("color: red;"); }
            qDebug() << "[Profile] socket未连接:" << err;
        }
    });
}

PageProfile::~PageProfile() {
    delete ui;
}
