
#include "page_dashboard.h"
#include "ui_page_dashboard.h"

#include "usercontext.h"
#include "main_page.h"
#include <QTimer>


PageDashboard::PageDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Dashboard) {
    ui->setupUi(this);
    QString styleSheet=R"(
        /* 主窗口样式 */
        Page_Dashboard {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                stop: 0 #f5f7fa, stop: 1 #c3cfe2);
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
        }

        /* 统计卡片样式 */
        QGroupBox {
            background: rgba(255, 255, 255, 0.9);
            border: 0.07em solid #bdc3c7;
            border-radius: 0.56em;
            margin: 0.35em;
            padding: 1.05em;
            font-weight: bold;
            color: #2c3e50;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 0.56em;
            background: #3498db;
            color: white;
            border-radius: 0.28em;
        }

        /* 统计数字样式 */
        QLabel#pending, QLabel#confirmed, QLabel#cancelled {
            font-size: 24pt;
            font-weight: bold;
            color: #2c3e50;
            qproperty-alignment: AlignCenter;
        }

        /* 表格样式 */
        QTableWidget {
            background: white;
            border: 0.07em solid #bdc3c7;
            border-radius: 0.35em;
            gridline-color: #e9ecef;
            alternate-background-color: #f8f9fa;
        }

        QTableWidget::item {
            padding: 0.56em;
            border: none;
            border-bottom: 0.07em solid #e9ecef;
        }

        QTableWidget::item:selected {
            background: #3498db;
            color: white;
        }

        QHeaderView::section {
            background: #3498db;
            color: white;
            padding: 0.56em;
            border: none;
            font-weight: bold;
        }

        /* 健康提示框样式 */
        #healthTipsBox {
            background: rgba(255, 255, 255, 0.9);
            border: 0.07em solid #bdc3c7;
            border-radius: 0.56em;
            padding: 1.05em;
        }

        #healthTipsBox::title {
            background: #2ecc71;
            color: white;
            border-radius: 0.28em;
            padding: 0 0.56em;
        }

        /* 健康提示标签样式 */
        #time, #level {
            font-size: 10.5pt;
            font-weight: bold;
            color: #2c3e50;
            padding: 0.35em 0;
            border-bottom: 0.07em solid #e9ecef;
            margin-bottom: 0.7em;
        }

        #labelTip1, #labelTip2, #labelTip3 {
            font-size: 9.5pt;
            color: #34495e;
            padding: 0.56em;
            margin: 0.35em 0;
            background: #f8f9fa;
            border-radius: 0.35em;
            border-left: 0.21em solid #3498db;
        }

        /* 布局调整 */
        QVBoxLayout#verticalLayout, QHBoxLayout#statsLayout, QHBoxLayout#mainContentLayout {
            margin: 0.7em;
            spacing: 1.05em;
        }

        /* 卡片布局 */
        QVBoxLayout#cardAppointmentLayout,
        QVBoxLayout#cardRecordsLayout,
        QVBoxLayout#cardMessagesLayout,
        QVBoxLayout#recentAppointmentLayout,
        QVBoxLayout#healthTipsLayout {
            margin: 0.35em;
            spacing: 0.7em;
        }
)";
    this->setStyleSheet(styleSheet);
    // 构造函数只做UI初始化，不做数据请求
    qDebug() << "[Dashboard] 构造完成，等待 fetchHealthResult()/fetchAppointments() 调用";
}

// 健康评估结果查询与UI展示
void PageDashboard::fetchHealthResult() {
    int user_id = UserContext::instance()->userId();
    qDebug() << "[Dashboard] fetchHealthResult: user_id=" << user_id;
    // 获取 Main_Page 的 socket
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
    qDebug() << "[Dashboard] fetchHealthResult: 获取到socket=" << socket;
    if (socket && socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
        QJsonObject req;
        req["type"] = "health.get";
        req["seq"] = 1047;
        req["user_id"] = user_id;
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Dashboard] fetchHealthResult: 发送到服务器:" << QString::fromUtf8(jsonData);
        socket->write(packet);
        socket->flush();
        bool filled = false;
        if (socket->waitForReadyRead(3000)) {
            QByteArray resp = socket->readAll();
            qDebug() << "[Dashboard] fetchHealthResult: 收到服务器反馈长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Dashboard] fetchHealthResult: 返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "health.get" && respObj.value("seq").toInt() == 1047) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        QString time = payload.value("time").toString();
                        QString risk_level = payload.value("risk_level").toString();
                        QJsonArray adviceArr = payload.value("advice").toArray();
                        // 检查字段是否为空
                        if (!time.isEmpty() && !risk_level.isEmpty() && adviceArr.size() > 0) {
                            qDebug() << "[Dashboard] 健康评估结果: time=" << time << ", risk_level=" << risk_level << ", adviceArr=" << adviceArr;
                            if (ui->time) ui->time->setText("评估时间:" + time);
                            if (ui->level) ui->level->setText("风险等级: " + risk_level);
                            if (ui->labelTip1) ui->labelTip1->setText(adviceArr.size() > 0 ? adviceArr[0].toString() : "");
                            if (ui->labelTip2) ui->labelTip2->setText(adviceArr.size() > 1 ? adviceArr[1].toString() : "");
                            if (ui->labelTip3) ui->labelTip3->setText(adviceArr.size() > 2 ? adviceArr[2].toString() : "");
                            filled = true;
                        }
                    } else {
                        qDebug() << "[Dashboard] fetchHealthResult: 服务器返回type/seq不匹配";
                    }
                } else {
                    qDebug() << "[Dashboard] fetchHealthResult: 返回json解析失败";
                }
            } else {
                qDebug() << "[Dashboard] fetchHealthResult: 返回数据长度异常";
            }
        } else {
            qDebug() << "[Dashboard] fetchHealthResult: 服务器无返回";
        }
        // 如果未填充，显示通用建议
        if (!filled) {
            QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
            QString risk = "无";
            QStringList tips = {"保持健康生活方式", "定期体检", "如有不适及时就医"};
            qDebug() << "[Dashboard] 健康评估无返回，填充默认建议: time=" << now << ", risk_level=" << risk << ", tips=" << tips;
            if (ui->time) ui->time->setText("评估时间:" + now);
            if (ui->level) ui->level->setText("风险等级: " + risk);
            if (ui->labelTip1) ui->labelTip1->setText(tips.value(0));
            if (ui->labelTip2) ui->labelTip2->setText(tips.value(1));
            if (ui->labelTip3) ui->labelTip3->setText(tips.value(2));
        }
    } else {
        qDebug() << "[Dashboard] fetchHealthResult: socket未连接或user_id无效";
        // 也填充默认建议
        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
        QString risk = "无";
        QStringList tips = {"保持健康生活方式", "定期体检", "如有不适及时就医"};
        qDebug() << "[Dashboard] 健康评估无返回，填充默认建议: time=" << now << ", risk_level=" << risk << ", tips=" << tips;
        if (ui->time) ui->time->setText("评估时间:" + now);
        if (ui->level) ui->level->setText("风险等级: " + risk);
        if (ui->labelTip1) ui->labelTip1->setText(tips.value(0));
        if (ui->labelTip2) ui->labelTip2->setText(tips.value(1));
        if (ui->labelTip3) ui->labelTip3->setText(tips.value(2));
    }
}

// 预约列表请求与解析，延迟调用
void PageDashboard::fetchAppointments() {
    // 获取当前用户ID
    int user_id = UserContext::instance()->userId();
    qDebug() << "[Dashboard] fetchAppointments: user_id=" << user_id;

    // 获取 Main_Page 的 socket
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
    qDebug() << "[Dashboard] fetchAppointments: 获取到socket=" << socket;
    int num_pending = 0;
    int num_confirmed = 0;
    int num_cancelled = 0;
    QJsonArray appointments;
    // 检查 socket 和 user_id 是否有效
    if (socket && socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
        // 构造请求 JSON
        QJsonObject req;
        req["type"] = "appt.list";
        req["seq"] = 1004;
        req["user_id"] = user_id;
        req["payload"] = QJsonObject();
        QJsonDocument doc(req);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_10);
        stream << static_cast<quint32>(jsonData.size());
        packet.append(jsonData);
        qDebug() << "[Dashboard] fetchAppointments: 发送到服务器:" << QString::fromUtf8(jsonData);
        socket->write(packet);
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            QByteArray resp = socket->readAll();
            qDebug() << "[Dashboard] fetchAppointments: 收到服务器反馈长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Dashboard] fetchAppointments: 返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if(false){
                        // ================== 测试数据区域（可注释/取消注释启用） ==================
                        // 说明：如需测试，无需服务器返回时可直接填充数据到UI
                        // 建议仅在开发/调试阶段启用，正式环境请注释掉此块
                        qDebug() << "[Dashboard] 使用本地测试数据填充UI";
                        num_pending = 2;
                        num_confirmed = 8;
                        num_cancelled = 1;
                        appointments.append(QJsonObject{{"appt_id", 101}, {"time", "2025-09-01 09:00"}, {"department_name", "心内科"}, {"doctor_name", "王医生"}, {"status", "待处理"}});
                        appointments.append(QJsonObject{{"appt_id", 102}, {"time", "2025-09-02 14:30"}, {"department_name", "神经科"}, {"doctor_name", "李医生"}, {"status", "已确认"}});
                        appointments.append(QJsonObject{{"appt_id", 103}, {"time", "2025-09-03 10:15"}, {"department_name", "呼吸科"}, {"doctor_name", "张医生"}, {"status", "已取消"}});
                        // ================== 测试数据区域结束 ==================
                    }
                    else if (respObj.value("seq").toInt() == 1004 && respObj.value("ok").toBool()) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        num_pending = payload.value("num_pending").toInt();
                        num_confirmed = payload.value("num_confirmed").toInt();
                        num_cancelled = payload.value("num_cancelled").toInt();
                        qDebug() << "[Dashboard] fetchAppointments: num_pending:" << num_pending;
                        qDebug() << "[Dashboard] fetchAppointments: num_confirmed:" << num_confirmed;
                        qDebug() << "[Dashboard] fetchAppointments: num_cancelled:" << num_cancelled;
                        appointments = payload.value("appointments").toArray();
                        qDebug() << "[Dashboard] fetchAppointments: appointments数量:" << appointments.size();
                        for (const QJsonValue &val : appointments) {
                            QJsonObject appt = val.toObject();
                            qDebug() << "[Dashboard] fetchAppointments: appt_id:" << appt.value("appt_id").toInt();
                            qDebug() << "[Dashboard] fetchAppointments: doctor_name:" << appt.value("doctor_name").toString();
                            qDebug() << "[Dashboard] fetchAppointments: department_name:" << appt.value("department_name").toString();
                            qDebug() << "[Dashboard] fetchAppointments: time:" << appt.value("time").toString();
                            qDebug() << "[Dashboard] fetchAppointments: status:" << appt.value("status").toString();
                        }
                    } else {
                        qDebug() << "[Dashboard] fetchAppointments: 服务器返回ok=false或seq不匹配";
                    }
                } else {
                    qDebug() << "[Dashboard] fetchAppointments: 返回json解析失败";
                }
            } else {
                qDebug() << "[Dashboard] fetchAppointments: 返回数据长度异常";
            }
        } else {
            qDebug() << "[Dashboard] fetchAppointments: 服务器无返回";
        }
    } else {
        qDebug() << "[Dashboard] fetchAppointments: socket未连接或user_id无效";
    }
    // ================== 统一UI赋值区域 ==================
    if (ui->pending) ui->pending->setText(QString::number(num_pending));
    if (ui->confirmed) ui->confirmed->setText(QString::number(num_confirmed));
    if (ui->cancelled) ui->cancelled->setText(QString::number(num_cancelled));

    if (ui->tableRecentAppointment) {
        ui->tableRecentAppointment->setRowCount(appointments.size());
        ui->tableRecentAppointment->setColumnCount(5);
        QStringList header;
        header << "id" << "时间" << "科室" << "医生" << "状态";
        ui->tableRecentAppointment->setHorizontalHeaderLabels(header);
        for (int i = 0; i < appointments.size(); ++i) {
            QJsonObject appt = appointments[i].toObject();
            ui->tableRecentAppointment->setItem(i, 0, new QTableWidgetItem(QString::number(appt.value("appt_id").toInt())));
            ui->tableRecentAppointment->setItem(i, 1, new QTableWidgetItem(appt.value("time").toString()));
            ui->tableRecentAppointment->setItem(i, 2, new QTableWidgetItem(appt.value("department_name").toString()));
            ui->tableRecentAppointment->setItem(i, 3, new QTableWidgetItem(appt.value("doctor_name").toString()));
            ui->tableRecentAppointment->setItem(i, 4, new QTableWidgetItem(appt.value("status").toString()));
        }
    }
}

PageDashboard::~PageDashboard() {
    delete ui;
}
