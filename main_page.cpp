#include "main_page.h"
#include "ui_main_page.h"
#include <QFile>
#include <QDebug>
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"

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
    // 初始化子页面
    pageAppointment = new PageAppointment(this);
    pageRecords = new PageRecords(this);
    pageChat = new PageChat(this);
    // 清空 mainStack，只保留首页
    while (ui->mainStack->count() > 1) {
        QWidget *w = ui->mainStack->widget(1);
        ui->mainStack->removeWidget(w);
        delete w;
    }
    // 添加新页面
    ui->mainStack->addWidget(pageAppointment);    // index 1
    ui->mainStack->addWidget(pageRecords);        // index 2
    ui->mainStack->addWidget(pageChat);           // index 3
    // 导航栏与页面切换逻辑
    connect(ui->navList, &QListWidget::currentRowChanged, this, [=](int row){
        if (row == 0) {
            ui->mainStack->setCurrentIndex(0);
            qDebug() << "首页";
        } else if (row == 1) {
            ui->mainStack->setCurrentIndex(1);
            qDebug() << "预约挂号";
        } else if (row == 2) {
            ui->mainStack->setCurrentIndex(2);
            qDebug() << "我的病例";
        } else if (row == 4) {
            ui->mainStack->setCurrentIndex(3);
            qDebug() << "医患沟通";
        } else {
            ui->mainStack->setCurrentIndex(0);
            qDebug() << "其他页面（未实现）";
        }
    });
    ui->navList->setCurrentRow(0); // 默认显示首页
}

Main_Page::~Main_Page()
{
    delete ui;
}
