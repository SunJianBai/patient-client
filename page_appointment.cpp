#include "page_appointment.h"
#include "ui_page_appointment.h"
#include "main_page.h"

PageAppointment::PageAppointment(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Appointment) {
    ui->setupUi(this);


    // 设置输入范围约束
    ui->age->setMinimum(1);
    ui->age->setMaximum(150);
    ui->height->setMinimum(50);
    ui->height->setMaximum(300);
    ui->weight->setMinimum(0);
    ui->weight->setMaximum(500);

    ui->comboDepartment->clear();
    ui->comboDoctor->clear();

    // 获取socket
    QTcpSocket *socket = nullptr;
    QWidget *p = parent;
    while (p) {
        Main_Page *mainPage = qobject_cast<Main_Page *>(p);
        if (mainPage) { socket = mainPage->m_socket; break; }
        p = p->parentWidget();
    }
    qDebug() << "[Appointment] 获取到socket:" << socket;

    // 请求科室列表
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject req;
        req["type"] = "department_list";
        req["seq"] = 1097;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Appointment] 发送科室列表请求:" << QString::fromUtf8(jsonData);
        socket->write(packet);
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QByteArray resp = socket->readAll();
            qDebug() << "[Appointment] 收到科室列表反馈长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Appointment] 科室列表返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "department_list" && respObj.value("seq").toInt() == 1097) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        QJsonArray arr = payload.value("appointments").toArray();
                        departmentList.clear();
                        for (const QJsonValue &val : arr) {
                            QString dept = val.toObject().value("department_name").toString();
                            if (!dept.isEmpty()) {
                                departmentList.append(dept);
                                qDebug() << "[Appointment] 获取科室:" << dept;
                            }
                        }
                        ui->comboDepartment->clear();
                        ui->comboDepartment->addItems(departmentList);
                    }
                }
            }
        } else {
            qDebug() << "[Appointment] 科室列表服务器无返回，使用测试数据";
            departmentList = QStringList{"呼吸科", "消化科", "心内科"};
            ui->comboDepartment->addItems(departmentList);
        }
    }

    // 科室选择变化时，清空医生并请求医生列表
    connect(ui->comboDepartment, &QComboBox::currentTextChanged, this, [=](const QString &dept){
        ui->comboDoctor->clear();
        doctorList.clear();
        qDebug() << "[Appointment] 选择科室:" << dept << "，清空医生列表并请求医生数据";
        if (socket && socket->state() == QAbstractSocket::ConnectedState && !dept.isEmpty()) {
            QJsonObject req;
            req["type"] = "doctor_list";
            req["seq"] = 1098;
            req["department_name"] = dept;
            QJsonDocument doc(req);
            QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
            QByteArray packet;
            QDataStream stream(&packet, QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_5_10);
            stream << static_cast<quint32>(jsonData.size());
            packet.append(jsonData);
            qDebug() << "[Appointment] 发送医生列表请求:" << QString::fromUtf8(jsonData);
            socket->write(packet);
            socket->flush();
            if (socket->waitForReadyRead(3000)) {
                QByteArray resp = socket->readAll();
                qDebug() << "[Appointment] 收到医生列表反馈长度:" << resp.size();
                if (resp.size() >= 4) {
                    QDataStream respStream(resp);
                    respStream.setVersion(QDataStream::Qt_5_10);
                    quint32 len = 0;
                    respStream >> len;
                    QByteArray jsonResp = resp.right(resp.size() - 4);
                    qDebug() << "[Appointment] 医生列表返回json:" << QString::fromUtf8(jsonResp);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                    if (respDoc.isObject()) {
                        QJsonObject respObj = respDoc.object();
                        if (respObj.value("type").toString() == "doctor_list" && respObj.value("seq").toInt() == 1098) {
                            QJsonObject payload = respObj.value("payload").toObject();
                            QJsonArray arr = payload.value("appointments").toArray();
                            for (const QJsonValue &val : arr) {
                                QString doctor = val.toObject().value("full_name").toString();
                                if (!doctor.isEmpty()) {
                                    doctorList.append(doctor);
                                    qDebug() << "[Appointment] 获取医生:" << doctor;
                                }
                            }
                            ui->comboDoctor->addItems(doctorList);
                        }
                    }
                }
            } else {
                qDebug() << "[Appointment] 医生列表服务器无返回，使用测试数据";
                doctorList = QStringList{"呼吸科", "消化科", "心内科"};
                ui->comboDoctor->addItems(doctorList);
            }
        }
    });

    // 提交按钮槽函数
    connect(ui->submitBtn, &QPushButton::clicked, this, [=]() {
        int age = ui->age->value();
        int height = ui->height->value();
        int weight = ui->weight->value();
        QString symptoms = ui->sympptoms->toPlainText();
        QString department = ui->comboDepartment->currentText();
        QString doctor = ui->comboDoctor->currentText();
        qDebug() << "[提交]:";
        qDebug() << "年龄:" << age;
        qDebug() << "身高:" << height;
        qDebug() << "体重:" << weight;
        qDebug() << "症状:" << symptoms;
        qDebug() << "科室:" << department;
        qDebug() << "医生:" << doctor;

        // 获取医生id（这里只做演示，实际应从医生列表获取id）
        int doctor_id = 2001; // 可根据实际医生选择做映射
        QString start_time = "2025-08-25 10:00"; // 可根据实际选择时间
        int user_id = 1001; // 实际应从全局用户信息获取

        // 构造JSON请求
        QJsonObject payload;
        payload["doctor_id"] = doctor_id;
        payload["start_time"] = start_time;
        payload["age"] = age;
        payload["height"] = height;
        payload["weight"] = weight;
        payload["sympptoms"] = symptoms;
        QJsonObject req;
        req["type"] = "appt.create";
        req["seq"] = 1003;
        req["user_id"] = user_id;
        req["payload"] = payload;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Appointment] 发送预约请求:" << QString::fromUtf8(jsonData);
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(packet);
            socket->flush();
            if (socket->waitForReadyRead(3000)) {
                QByteArray resp = socket->readAll();
                qDebug() << "[Appointment] 收到预约反馈长度:" << resp.size();
                if (resp.size() >= 4) {
                    QDataStream respStream(resp);
                    respStream.setVersion(QDataStream::Qt_5_10);
                    quint32 len = 0;
                    respStream >> len;
                    QByteArray jsonResp = resp.right(resp.size() - 4);
                    qDebug() << "[Appointment] 预约返回json:" << QString::fromUtf8(jsonResp);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                    if (respDoc.isObject()) {
                        QJsonObject respObj = respDoc.object();
                        if (respObj.value("type").toString() == "appt.create" && respObj.value("seq").toInt() == 1003) {
                            bool ok = respObj.value("ok").toBool();
                            qDebug() << "[Appointment] 预约结果ok:" << ok;
                            if (ok) {
                                QMessageBox::information(this, "预约成功", "您的预约已成功提交！");
                            } else {
                                QMessageBox::warning(this, "预约失败", "预约提交失败，请重试！");
                            }
                        }
                    }
                }
            } else {
                qDebug() << "[Appointment] 预约服务器无返回";
                QMessageBox::warning(this, "预约失败", "服务器无响应，预约失败！");
            }
        } else {
            qDebug() << "[Appointment] socket未连接，无法预约";
            QMessageBox::warning(this, "网络错误", "未连接到服务器，无法预约！");
        }
    });
}



PageAppointment::~PageAppointment() {
    delete ui;
}
