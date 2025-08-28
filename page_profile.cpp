#include "page_profile.h"
#include "ui_page_profile.h"
PageProfile::PageProfile(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Profile) {
    ui->setupUi(this);
}
PageProfile::~PageProfile() {
    delete ui;
}
