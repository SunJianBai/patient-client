#include "page_dashboard.h"
#include "ui_page_dashboard.h"
PageDashboard::PageDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Dashboard) {
    ui->setupUi(this);
}
PageDashboard::~PageDashboard() {
    delete ui;
}
