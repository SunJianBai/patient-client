#include "page_chat.h"
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
PageChat::PageChat(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("医患沟通页面", this);
    layout->addWidget(label);
}
