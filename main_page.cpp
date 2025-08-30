#include "main_page.h"
#include "ui_main_page.h"



Main_Page::Main_Page(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Page)
{
    ui->setupUi(this);

    // ...后续UI和页面初始化逻辑...
    // 加载全局样式表
    QFile qssFile(":/style/resources/global.qss");
    if (qssFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(style);
        qssFile.close();
    }
    // 初始化所有子页面
    pageDashboard = new PageDashboard(this);
    pageAppointment = new PageAppointment(this);
    pageRecords = new PageRecords(this);
    pageChat = new PageChat(this);
    pageAssessment = new PageAssessment(this);
    pageProfile = new PageProfile(this);
    
    // 清空 mainStack
    while (ui->mainStack->count() > 0) {
        QWidget *w = ui->mainStack->widget(0);
        ui->mainStack->removeWidget(w);
        delete w;
    }
    // 按导航栏顺序添加所有页面
    ui->mainStack->addWidget(pageDashboard);      // index 0 首页
    ui->mainStack->addWidget(pageAppointment);    // index 1 预约挂号
    ui->mainStack->addWidget(pageRecords);        // index 2 我的病例
    ui->mainStack->addWidget(pageChat);           // index 3 医患沟通
    ui->mainStack->addWidget(pageAssessment);     // index 4 健康评估
    ui->mainStack->addWidget(pageProfile);        // index 5 个人中心
    // 导航栏与页面切换逻辑
    connect(ui->navList, &QListWidget::currentRowChanged, this, [=](int row){
        ui->mainStack->setCurrentIndex(row);
        switch(row) {
            case 0: qDebug() << "首页"; break;
            case 1: qDebug() << "预约挂号"; break;
            case 2: qDebug() << "我的病例"; break;
            case 3: qDebug() << "医患沟通"; break;
            case 4: qDebug() << "健康评估"; break;
            case 5: qDebug() << "个人中心"; break;
            default: qDebug() << "未知页面"; break;
        }
    });
    ui->navList->setCurrentRow(0); // 默认显示首页
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
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "userinfo" && respObj.value("seq").toInt() == 1099) {
                        QJsonObject payload = respObj.value("payload").toObject();
                        UserContext::instance()->setUserId(user_id);
                        UserContext::instance()->setFromJson(payload);
                        qDebug() << "个人信息已保存:" << payload;
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
