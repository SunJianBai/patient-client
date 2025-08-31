#include "page_records.h"
#include "ui_page_records.h"
#include "usercontext.h"
#include "main_page.h"
#include <QListWidgetItem>
#include <QJsonArray>

PageRecords::PageRecords(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Records) {
    ui->setupUi(this);
    ui->listWidget->setMinimumWidth(250);
    ui->listWidget->setMaximumWidth(250);
    // 获取socket
    QTcpSocket *socket = nullptr;
    QWidget *p = parent;
    while (p) {
        Main_Page *mainPage = qobject_cast<Main_Page *>(p);
        if (mainPage) { socket = mainPage->m_socket; break; }
        p = p->parentWidget();
    }
    qDebug() << "[Records] 获取到socket:" << socket;

    // 初始化时请求预约列表
    int user_id = UserContext::instance()->userId();
    qDebug() << "[Records] 请求预约列表，user_id=" << user_id;
    QMap<int, QJsonObject> apptMap; // appt_id -> appt info
    if (socket && socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
        QJsonObject req;
        req["type"] = "record.list";
        req["seq"] = 1096;
        req["user_id"] = user_id;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Records] 发送预约列表请求:" << QString::fromUtf8(jsonData);
        socket->write(packet);
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QByteArray resp = socket->readAll();
            qDebug() << "[Records] 收到预约列表反馈长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Records] 预约列表返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "record.list" && respObj.value("seq").toInt() == 1096) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        QJsonArray arr = payload.value("appointments").toArray();
                        ui->listWidget->clear();
                        apptMap.clear();
                        for (const QJsonValue &val : arr) {
                            QJsonObject appt = val.toObject();
                            int appt_id = appt.value("appt_id").toInt();
                            QString time = appt.value("time").toString();
                            QString doctor = appt.value("doctor_name").toString();
                            QString dept = appt.value("department_name").toString();
                            apptMap[appt_id] = appt;
                            QListWidgetItem *item = new QListWidgetItem(time, ui->listWidget);
                            item->setData(Qt::UserRole, appt_id);
                            qDebug() << "[Records] 添加预约: appt_id=" << appt_id << ", time=" << time << ", doctor=" << doctor << ", dept=" << dept;
                        }
                    }
                }
            }
        } else {
            qDebug() << "[Records] 预约列表服务器无返回";
        }
    } else {
        qDebug() << "[Records] socket未连接或user_id无效";
    }

    // 选择预约时间后，请求详细记录
    connect(ui->listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        int appt_id = item->data(Qt::UserRole).toInt();
        qDebug() << "[Records] 选中预约，appt_id=" << appt_id;
        // 发送详细记录请求
        if (socket && socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
            QJsonObject req;
            req["type"] = "record";
            req["seq"] = 1006;
            req["user_id"] = user_id;
            req["appt_id"] = appt_id;
            QJsonDocument doc(req);
            QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
            QByteArray packet;
            QDataStream stream(&packet, QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_5_10);
            stream << static_cast<quint32>(jsonData.size());
            packet.append(jsonData);
            qDebug() << "[Records] 发送详细记录请求:" << QString::fromUtf8(jsonData);
            socket->write(packet);
            socket->flush();
            if (socket->waitForReadyRead(3000)) {
                QByteArray resp = socket->readAll();
                qDebug() << "[Records] 收到详细记录反馈长度:" << resp.size();
                if (resp.size() >= 4) {
                    QDataStream respStream(resp);
                    respStream.setVersion(QDataStream::Qt_5_10);
                    quint32 len = 0;
                    respStream >> len;
                    QByteArray jsonResp = resp.right(resp.size() - 4);
                    qDebug() << "[Records] 详细记录返回json:" << QString::fromUtf8(jsonResp);
                    QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                    if (respDoc.isObject()) {
                        QJsonObject respObj = respDoc.object();
                        if (respObj.value("type").toString() == "record" && respObj.value("seq").toInt() == 1006) {
                            QJsonObject payload = respObj.value("payload").toObject();
                            QString doctor = payload.value("doctor_name").toString();
                            QString dept = payload.value("department_name").toString();
                            QString symp = payload.value("sympptoms").toString();
                            QString pres = payload.value("prescription").toString();
                            qDebug() << "[Records] 详细记录: doctor=" << doctor << ", dept=" << dept << ", symp=" << symp << ", pres=" << pres;
                            ui->doctor_name->setText(doctor);
                            ui->department_name->setText(dept);
                            ui->sympptoms->setText(symp);
                            ui->prescription->setText(pres);
                        }
                    }
                }
            } else {
                qDebug() << "[Records] 详细记录服务器无返回";
            }
        } else {
            qDebug() << "[Records] socket未连接或user_id无效，无法请求详细记录";
        }
    });
}

PageRecords::~PageRecords() {
    delete ui;
}
