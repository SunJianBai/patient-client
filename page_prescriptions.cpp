#include "page_prescriptions.h"
#include "ui_page_prescriptions.h"
PagePrescriptions::PagePrescriptions(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Prescriptions) {
    ui->setupUi(this);
}
PagePrescriptions::~PagePrescriptions() {
    delete ui;
}
