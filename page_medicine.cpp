#include "page_medicine.h"
#include "ui_page_medicine.h"
PageMedicine::PageMedicine(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Medicine) {
    ui->setupUi(this);
}
PageMedicine::~PageMedicine() {
    delete ui;
}
