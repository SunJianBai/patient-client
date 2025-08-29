#include "page_assessment.h"
#include "ui_page_assessment.h"
PageAssessment::PageAssessment(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Assessment) {
    ui->setupUi(this);
}
PageAssessment::~PageAssessment() {
    delete ui;
}
