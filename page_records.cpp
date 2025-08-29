#include "page_records.h"
#include "ui_page_records.h"
PageRecords::PageRecords(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Records) {
    ui->setupUi(this);
}
PageRecords::~PageRecords() {
    delete ui;
}
