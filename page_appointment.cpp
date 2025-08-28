#include "page_appointment.h"
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
PageAppointment::PageAppointment(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("预约挂号页面", this);
    layout->addWidget(label);
}
