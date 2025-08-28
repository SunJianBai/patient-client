#include "page_records.h"
#include <QLabel>
#include <QVBoxLayout>
PageRecords::PageRecords(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("我的病例页面", this);
    layout->addWidget(label);
}
