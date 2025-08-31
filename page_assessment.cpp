#include "page_assessment.h"
#include "ui_page_assessment.h"
#include "datetime_utils.h"
#include "main_page.h"

PageAssessment::PageAssessment(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Assessment) {
    ui->setupUi(this);
    connect(ui->btn_submit, &QPushButton::clicked, this, &PageAssessment::onSubmitClicked);
}
PageAssessment::~PageAssessment() {
    delete ui;
}

void PageAssessment::onSubmitClicked() {
    // 完整性校验：所有ComboBox必须有选项，慢性病史和家族病史至少选一个
    if (ui->combo_age->currentIndex() < 0 ||
        ui->combo_gender->currentIndex() < 0 ||
        ui->combo_bmi->currentIndex() < 0 ||
        ui->combo_sleep->currentIndex() < 0 ||
        ui->combo_sleepquality->currentIndex() < 0 ||
        ui->combo_smoke->currentIndex() < 0 ||
        ui->combo_drink->currentIndex() < 0 ||
        ui->combo_exercise->currentIndex() < 0 ||
        ui->combo_vege->currentIndex() < 0 ||
        ui->combo_fastfood->currentIndex() < 0 ||
        ui->combo_stress->currentIndex() < 0 ||
        ui->combo_emotion->currentIndex() < 0 ||
        ui->combo_dailywork->currentIndex() < 0) {
        QMessageBox::warning(this, "有选项为空", "请完整填写所有选项！");
        return;
    }
    // 慢性病史
    if (!ui->chk_hypertension->isChecked() && !ui->chk_diabetes->isChecked() &&
        !ui->chk_heart->isChecked() && !ui->chk_none->isChecked()) {
        QMessageBox::warning(this, "有选项为空", "请至少选择一项慢性疾病史！");
        return;
    }
    // 家族病史
    if (!ui->chk_family_hypertension->isChecked() && !ui->chk_family_diabetes->isChecked() &&
        !ui->chk_family_heart->isChecked() && !ui->chk_family_none->isChecked()) {
        QMessageBox::warning(this, "有选项为空", "请至少选择一项家族病史！");
        return;
    }

    QStringList lines;
    // 基本信息
    lines << "一、基本信息";
    lines << QString("1. 您的年龄范围是？ %1").arg(ui->combo_age->currentText());
    lines << QString("2. 您的性别是？ %1").arg(ui->combo_gender->currentText());
    lines << QString("3. 您的体重状况？（自报） %1").arg(ui->combo_bmi->currentText());

    // 生活方式
    lines << "\n二、生活方式";
    lines << QString("4. 您平均每天的睡眠时间是？ %1").arg(ui->combo_sleep->currentText());
    lines << QString("5. 您的睡眠质量如何？ %1").arg(ui->combo_sleepquality->currentText());
    lines << QString("6. 您是否有吸烟习惯？ %1").arg(ui->combo_smoke->currentText());
    lines << QString("7. 您是否有饮酒习惯？ %1").arg(ui->combo_drink->currentText());
    lines << QString("8. 您每周的运动频率是？ %1").arg(ui->combo_exercise->currentText());

    // 饮食习惯
    lines << "\n三、饮食习惯";
    lines << QString("9. 您平均每天吃蔬菜的次数？ %1").arg(ui->combo_vege->currentText());
    lines << QString("10. 您吃快餐/外卖的频率？ %1").arg(ui->combo_fastfood->currentText());

    // 慢性病史
    lines << "\n四、慢性疾病史";
    QStringList chronic;
    if(ui->chk_hypertension->isChecked()) chronic << "高血压";
    if(ui->chk_diabetes->isChecked()) chronic << "糖尿病";
    if(ui->chk_heart->isChecked()) chronic << "心脏病";
    if(ui->chk_none->isChecked()) chronic << "无";
    lines << QString("慢性疾病史: %1").arg(chronic.isEmpty() ? "无" : chronic.join("、"));

    // 家族病史
    lines << "\n五、家族病史";
    QStringList family;
    if(ui->chk_family_hypertension->isChecked()) family << "高血压";
    if(ui->chk_family_diabetes->isChecked()) family << "糖尿病";
    if(ui->chk_family_heart->isChecked()) family << "心脏病/中风";
    if(ui->chk_family_none->isChecked()) family << "无";
    lines << QString("家族病史: %1").arg(family.isEmpty() ? "无" : family.join("、"));

    // 心理健康
    lines << "\n六、心理健康";
    lines << QString("11. 您最近是否经常感到压力大？ %1").arg(ui->combo_stress->currentText());
    lines << QString("12. 您是否经常感到情绪低落？ %1").arg(ui->combo_emotion->currentText());

    // 体力与日常活动
    lines << "\n七、体力与日常活动";
    lines << QString("13. 您是否能够独立完成日常工作（如买菜、做饭、清洁）？ %1").arg(ui->combo_dailywork->currentText());

    QString result = lines.join("\n");
    qDebug() << "[健康评估问卷结果]\n" << result;

    // 运行时生成临时py文件并调用
    QString pyPath = "python3";
    QString pyCode =
        "import requests\n"
        "def ask_ai(question, prompt=None):\n"
        "    url = \"https://api.siliconflow.cn/v1/chat/completions\"\n"
        "    token = \"sk-cnaezfpjfsjptfogdusdjsanqxlptrdhyagrifilhthevspn\"\n"
        "    if prompt is None:\n"
        "        prompt = '''\n你是一名专业健康管理 AI，请根据以下健康调查问卷信息进行风险评估与建议。\n\n任务要求\n\n阅读用户的完整问卷信息。\n\n判定健康风险等级（只能为以下之一：无, 低, 中, 高, 极高）。\n\n给出 3 条简短的健康建议（每条 ≤ 20 字）。\n\n严格返回 JSON 格式，禁止输出额外说明。\n\n风险等级判定逻辑（参考，不可忽略）\n\n无：各项指标良好，无慢性病或家族病史，生活方式健康。\n\n低：轻微不良习惯（如偶尔缺乏运动、饮食略不均衡），无慢性病。\n\n中：存在多项不良习惯（如睡眠不足+不运动+饮食差），但无重大疾病。\n\n高：已有慢性疾病 或 家族病史 + 不良生活习惯。\n\n极高：多项严重风险叠加（如糖尿病+家族史+长期睡眠不足+不运动+饮食差）。\n\n用户问卷信息\n一、基本信息\n1. 您的年龄范围是？ 18岁以下\n2. 您的性别是？ 男\n3. 您的体重状况？（自报） 偏瘦\n\n二、生活方式\n4. 您平均每天的睡眠时间是？ 少于5小时\n5. 您的睡眠质量如何？ 很好\n6. 您是否有吸烟习惯？ 从不吸烟\n7. 您是否有饮酒习惯？ 从不饮酒\n8. 您每周的运动频率是？ 几乎不运动\n\n三、饮食习惯\n9. 您平均每天吃蔬菜的次数？ 几乎不吃\n10. 您吃快餐/外卖的频率？ 几乎不吃\n\n四、慢性疾病史\n慢性疾病史: 糖尿病\n\n五、家族病史\n家族病史: 糖尿病\n\n六、心理健康\n11. 您最近是否经常感到压力大？ 从不\n12. 您是否经常感到情绪低落？ 从不\n\n七、体力与日常活动\n13. 您是否能够独立完成日常工作（如买菜、做饭、清洁）？ 完全可以\n\n输出格式\n{\n  \"risk_level\": \"高\",\n  \"advice\": [\n    \"建议规律作息\",\n    \"增加适度运动\",\n    \"定期监测血糖\"\n  ]\n}\n\n'''\n"
        "    payload = {\n"
        "        'model': 'deepseek-ai/DeepSeek-V3.1',\n"
        "        'messages': [\n"
        "            {\n"
        "                'role': 'user',\n"
        "                'content': prompt + '\\n' + question\n"
        "            }\n"
        "        ]\n"
        "    }\n"
        "    headers = {\n"
        "        'Authorization': f'Bearer {token}',\n"
        "        'Content-Type': 'application/json'\n"
        "    }\n"
        "    response = requests.post(url, json=payload, headers=headers)\n"
        "    data = response.json()\n"
        "    try:\n"
        "        reply = data['choices'][0]['message']['content']\n"
        "    except Exception:\n"
        "        reply = data\n"
        "    return reply\n"
        "if __name__ == '__main__':\n"
        "    import sys\n"
        "    if len(sys.argv) > 1:\n"
        "        question = sys.argv[1]\n"
        "        prompt = None\n"
        "    print(ask_ai(question, prompt))\n";

    // 写入临时py文件
    QString tmpPy = QDir::temp().filePath("qt_ai_assess_temp.py");
    QFile f(tmpPy);
    if(f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        f.write(pyCode.toUtf8());
        f.close();
    }

    // 弹出模态等待框，允许用户主动关闭
    QMessageBox *waitBox = new QMessageBox(QMessageBox::Information, "健康评测中……", "正在为您分析健康状况，请稍候……", QMessageBox::Ok, this);
    waitBox->setWindowModality(Qt::ApplicationModal);
    waitBox->setAttribute(Qt::WA_DeleteOnClose);
    // 不再移除关闭按钮，允许用户主动关闭
    waitBox->show();

    QProcess *process = new QProcess(this);
    QStringList args;
    args << tmpPy << result;
    process->start(pyPath, args);

    // 只处理一次AI输出，避免多次弹窗
    bool *handled = new bool(false); // 动态分配，避免lambda捕获局部变量悬空
    QString *aiMsg = new QString;
    QString *risk_level = new QString;
    QJsonArray *adviceArr = new QJsonArray;
    bool *valid = new bool(false);
    connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
        if (*handled) return;
        QByteArray output = process->readAllStandardOutput();
        qDebug() << "[AI健康评估原始输出]" << QString::fromUtf8(output);
        QJsonDocument doc = QJsonDocument::fromJson(output);
        if(doc.isObject()) {
            QJsonObject obj = doc.object();
            *risk_level = obj.value("risk_level").toString();
            *adviceArr = obj.value("advice").toArray();
            QStringList advList;
            for(const QJsonValue &v : *adviceArr) advList << v.toString();
            *aiMsg = QString("风险等级：%1\n建议：\n%2").arg(*risk_level, advList.join("\n"));
            *valid = !risk_level->isEmpty() && !adviceArr->isEmpty();
        } else {
            *aiMsg = QString::fromUtf8(output);
        }
        qDebug() << "[AI健康评估]" << *aiMsg;
        *handled = true;
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus){
        // 先确保waitBox可关闭（即使异常也能关闭）
        if (waitBox->isVisible()) waitBox->close();
        int ret = QMessageBox::information(this, "AI健康评估", "已获得健康提示，请在首页查看");
        // 再次确保waitBox彻底关闭
        if (waitBox->isVisible()) waitBox->close();
        // 发送到服务器
        if(*valid) {
            QJsonObject payload;
            payload["type"] = "health.submit";
            static int seq = 1048;
            payload["seq"] = seq;
            int user_id = UserContext::instance()->userId();
            payload["user_id"] = QString::number(user_id);
            QJsonObject pl;
            pl["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
            pl["risk_level"] = *risk_level;
            pl["advice"] = *adviceArr;
            payload["payload"] = pl;
            QJsonDocument sendDoc(payload);
            QByteArray jsonData = sendDoc.toJson(QJsonDocument::Compact);
            QByteArray packet;
            QDataStream stream(&packet, QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_5_10);
            stream << static_cast<quint32>(jsonData.size());
            packet.append(jsonData);
            // 仿照预约界面获取socket
            QTcpSocket *socket = nullptr;
            QWidget *p = this->parentWidget();
            while (p) {
                Main_Page *mainPage = qobject_cast<Main_Page *>(p);
                if (mainPage) { socket = mainPage->m_socket; break; }
                p = p->parentWidget();
            }
            qDebug() << "[健康评估上报] user_id:" << user_id << ", socket:" << socket;
            qDebug() << "[健康评估上报] 发送内容:" << QString::fromUtf8(jsonData);
            if (socket && socket->state() == QAbstractSocket::ConnectedState) {
                socket->write(packet);
                socket->flush();
                if (socket->waitForReadyRead(3000)) {
                    QByteArray resp = socket->readAll();
                    qDebug() << "[健康评估上报] 收到服务器反馈长度:" << resp.size();
                    if (resp.size() >= 4) {
                        QDataStream respStream(resp);
                        respStream.setVersion(QDataStream::Qt_5_10);
                        quint32 len = 0;
                        respStream >> len;
                        QByteArray jsonResp = resp.right(resp.size() - 4);
                        qDebug() << "[健康评估上报] 服务器返回json:" << QString::fromUtf8(jsonResp);
                        QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                        if (respDoc.isObject()) {
                            QJsonObject respObj = respDoc.object();
                            bool ok = respObj.value("ok").toBool();
                            int rseq = respObj.value("seq").toInt();
                            qDebug() << "[健康评估上报] 结果ok:" << ok << ", seq:" << rseq;
                        }
                    }
                } else {
                    qDebug() << "[健康评估上报] 服务器无返回";
                }
            } else {
                qDebug() << "[健康评估上报] 未找到QTcpSocket，未发送";
            }
        }
        delete handled;
        delete aiMsg;
        delete risk_level;
        delete adviceArr;
        delete valid;
    });
    connect(process, &QProcess::readyReadStandardError, this, [process, this, waitBox]() {
        QByteArray err = process->readAllStandardError();
        if(!err.isEmpty()) {
            waitBox->close();
            QMessageBox::warning(this, "AI评估失败", QString::fromUtf8(err));
        }
    });
}
