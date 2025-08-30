#include "page_appointment.h"
#include "ui_page_appointment.h"


PageAppointment::PageAppointment(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Appointment) {
    ui->setupUi(this);

    // 初始化科室和医生选择项
    ui->comboDepartment->addItem("内科");
    ui->comboDepartment->addItem("外科");
    ui->comboDoctor->addItem("张医生");
    ui->comboDoctor->addItem("李医生");

    // 提交按钮槽函数
    connect(ui->submitBtn, &QPushButton::clicked, this, [=]() {
        int age = ui->age->value();
        int height = ui->height->value();
        int weight = ui->weight->value();
        QString symptoms = ui->sympptoms->toPlainText();
        QString department = ui->comboDepartment->currentText();
        QString doctor = ui->comboDoctor->currentText();
        qDebug() << "年龄:" << age;
        qDebug() << "身高:" << height;
        qDebug() << "体重:" << weight;
        qDebug() << "症状:" << symptoms;
        qDebug() << "科室:" << department;
        qDebug() << "医生:" << doctor;
    });
}

PageAppointment::~PageAppointment() {
    delete ui;
}
