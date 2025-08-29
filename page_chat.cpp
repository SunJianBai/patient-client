#include "page_chat.h"
#include "ui_page_chat.h"
PageChat::PageChat(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Chat) {
    ui->setupUi(this);
}
PageChat::~PageChat() {
    delete ui;
}
