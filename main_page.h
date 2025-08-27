#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include <QMainWindow>

namespace Ui {
class Main_Page;
}

class Main_Page : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Page(QWidget *parent = nullptr);
    ~Main_Page();

private:
    Ui::Main_Page *ui;
};

#endif // MAIN_PAGE_H
