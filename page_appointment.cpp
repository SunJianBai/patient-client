#include "page_appointment.h"
#include "ui_page_appointment.h"

PageAppointment::PageAppointment(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Appointment) {
    ui->setupUi(this);
}

PageAppointment::~PageAppointment() {
    delete ui;
}
