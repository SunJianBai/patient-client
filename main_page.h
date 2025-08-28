#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include <QMainWindow>
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"

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
    PageAppointment *pageAppointment;
    PageRecords *pageRecords;
    PageChat *pageChat;
};

#endif // MAIN_PAGE_H
