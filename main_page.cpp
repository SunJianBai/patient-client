#include "main_page.h"
#include "ui_main_page.h"
#include "page_dashboard.h"
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"
#include "page_prescriptions.h"
#include "page_assessment.h"
#include "page_medicine.h"
#include "page_profile.h"
#include <QFile>
#include <QDebug>

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
    // 初始化所有子页面
    PageDashboard *pageDashboard = new PageDashboard(this);
    PageAppointment *pageAppointment = new PageAppointment(this);
    PageRecords *pageRecords = new PageRecords(this);
    PagePrescriptions *pagePrescriptions = new PagePrescriptions(this);
    PageChat *pageChat = new PageChat(this);
    PageAssessment *pageAssessment = new PageAssessment(this);
    PageMedicine *pageMedicine = new PageMedicine(this);
    PageProfile *pageProfile = new PageProfile(this);
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
    ui->mainStack->addWidget(pagePrescriptions);  // index 3 我的医嘱
    ui->mainStack->addWidget(pageChat);           // index 4 医患沟通
    ui->mainStack->addWidget(pageAssessment);     // index 5 健康评估
    ui->mainStack->addWidget(pageMedicine);       // index 6 药品搜索
    ui->mainStack->addWidget(pageProfile);        // index 7 个人中心
    // 导航栏与页面切换逻辑
    connect(ui->navList, &QListWidget::currentRowChanged, this, [=](int row){
        ui->mainStack->setCurrentIndex(row);
        switch(row) {
            case 0: qDebug() << "首页"; break;
            case 1: qDebug() << "预约挂号"; break;
            case 2: qDebug() << "我的病例"; break;
            case 3: qDebug() << "我的医嘱"; break;
            case 4: qDebug() << "医患沟通"; break;
            case 5: qDebug() << "健康评估"; break;
            case 6: qDebug() << "药品搜索"; break;
            case 7: qDebug() << "个人中心"; break;
            default: qDebug() << "未知页面"; break;
        }
    });
    ui->navList->setCurrentRow(0); // 默认显示首页
}

Main_Page::~Main_Page()
{
    delete ui;
}
