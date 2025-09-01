
#include "main_page.h"
#include "ui_main_page.h"

Main_Page::Main_Page(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Page)
{
    ui->setupUi(this);
    // 加载全局样式表
    QFile qssFile(":/style/resources/global.qss");
    if (qssFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(style);
        qssFile.close();
    }

    // 清空 mainStack
    while (ui->mainStack->count() > 0) {
        QWidget *w = ui->mainStack->widget(0);
        ui->mainStack->removeWidget(w);
        delete w;
    }

    // 动态设置sidebar患者名称和ID
    ui->userName->setText(UserContext::instance()->name());
    ui->userId->setText(QString("ID: %1").arg(UserContext::instance()->userId()));
    qDebug() << "[Main_Page] sidebar初始化: name=" << UserContext::instance()->name() << ", user_id=" << UserContext::instance()->userId();

    // 延迟初始化所有子页面，首次切换时才new
    connect(ui->navList, &QListWidget::currentRowChanged, this, [=](int row){
        QWidget *targetPage = nullptr;
        qDebug() << "[Main_Page] 导航栏切换到:" << row;
        switch(row) {
            case 0:
                if (!pageDashboard) {
                    pageDashboard = new PageDashboard(this);
                    ui->mainStack->addWidget(pageDashboard);
                    qDebug() << "[Main_Page] 初始化首页 PageDashboard";
                }
                targetPage = pageDashboard;
                if (pageDashboard) {
                    pageDashboard->fetchHealthResult(); // 每次切换都请求健康评估
                    pageDashboard->fetchAppointments();
                }
                qDebug() << "首页"; break;
            case 1:
                if (!pageAppointment) {
                    pageAppointment = new PageAppointment(this);
                    ui->mainStack->addWidget(pageAppointment);
                    qDebug() << "[Main_Page] 初始化预约挂号 PageAppointment";
                }
                targetPage = pageAppointment;
                qDebug() << "预约挂号"; break;
            case 2:
                if (!pageRecords) {
                    pageRecords = new PageRecords(this);
                    ui->mainStack->addWidget(pageRecords);
                    qDebug() << "[Main_Page] 初始化病例 PageRecords";
                }
                targetPage = pageRecords;
                // 每次切换都刷新病例
                if (pageRecords) {
                    qDebug() << "[Main_Page] 刷新病例界面，自动请求数据";
                    QMetaObject::invokeMethod(pageRecords, "refreshRecords");
                }
                qDebug() << "我的病例"; break;
            case 3:
                if (!pageChat) {
                    pageChat = new PageChat(this);
                    ui->mainStack->addWidget(pageChat);
                    qDebug() << "[Main_Page] 初始化医患沟通 PageChat";
                }
                targetPage = pageChat;
                qDebug() << "医患沟通"; break;
            case 4:
                if (!pageAssessment) {
                    pageAssessment = new PageAssessment(this);
                    ui->mainStack->addWidget(pageAssessment);
                    qDebug() << "[Main_Page] 初始化健康评估 PageAssessment";
                }
                targetPage = pageAssessment;
                qDebug() << "健康评估"; break;
            case 5:
                if (!pageProfile) {
                    pageProfile = new PageProfile(this);
                    ui->mainStack->addWidget(pageProfile);
                    qDebug() << "[Main_Page] 初始化个人中心 PageProfile";
                }
                targetPage = pageProfile;
                // 每次切换都刷新个人信息
                if (pageProfile) {
                    qDebug() << "[Main_Page] 刷新个人中心界面，自动请求数据";
                    QMetaObject::invokeMethod(pageProfile, "refreshUserInfo");
                }
                qDebug() << "个人中心"; break;
            default:
                qDebug() << "未知页面"; break;
        }
        if (targetPage)
            ui->mainStack->setCurrentWidget(targetPage);
    });

    // 默认不自动切换首页，需登录成功后再切换

    // 退出登录按钮逻辑
    connect(ui->logoutBtn, &QPushButton::clicked, this, [=](){
        this->hide();
        QWidget *loginWin = nullptr;
        // 查找主窗口（登录窗口）并显示
        for (QWidget *w : QApplication::topLevelWidgets()) {
            if (w->inherits("MainWindow")) {
                loginWin = w;
                break;
            }
        }
        if (loginWin) loginWin->show();
    });
}

void Main_Page::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    int user_id = UserContext::instance()->userId();
    qDebug() << "[Main_Page] setSocket, user_id=" << user_id << ", socket=" << m_socket;
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState && user_id > 0) {
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
        qDebug() << "[Main_Page] 发送到服务器:" << QString::fromUtf8(jsonData);
        m_socket->write(packet);
        m_socket->flush();
        if (m_socket->waitForReadyRead(3000)) {
            QByteArray resp = m_socket->readAll();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Main_Page] 收到服务器反馈:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "userinfo" && respObj.value("seq").toInt() == 1099) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        UserContext::instance()->setUserId(user_id);
                        UserContext::instance()->setFromJson(payload);
                        // debug输出所有个人信息（只在成功获取后输出）
                        qDebug() << "获取到患者信息:";
                        qDebug() << "user:" << UserContext::instance()->user();
                        qDebug() << "role:" << UserContext::instance()->role();
                        qDebug() << "name:" << UserContext::instance()->name();
                        qDebug() << "gender:" << UserContext::instance()->gender();
                        qDebug() << "phone:" << UserContext::instance()->phone();
                        qDebug() << "id_number:" << UserContext::instance()->idNumber();
                        qDebug() << "adress:" << UserContext::instance()->adress();
                        qDebug() << "user_id:" << UserContext::instance()->userId();
                        qDebug() << "个人信息已保存:" << payload;
                        // 刷新sidebar显示
                        ui->userName->setText(UserContext::instance()->name());
                        ui->userId->setText(QString("ID: %1").arg(UserContext::instance()->userId()));
                    }
                }
            }
        }
    }
}

Main_Page::~Main_Page()
{
    delete ui;
}
