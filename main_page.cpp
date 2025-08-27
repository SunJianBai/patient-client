#include "main_page.h"
#include "ui_main_page.h"

Main_Page::Main_Page(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Page)
{
    ui->setupUi(this);
}

Main_Page::~Main_Page()
{
    delete ui;
}
