#include "page_dashboard.h"
#include "ui_page_dashboard.h"
#include "usercontext.h"

PageDashboard::PageDashboard(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Dashboard) {
    ui->setupUi(this);
    int user_id = UserContext::instance()->userId();
    qDebug() << "user_id:" << user_id;
}
PageDashboard::~PageDashboard() {
    delete ui;
}
