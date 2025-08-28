#ifndef PAGE_APPOINTMENT_H
#define PAGE_APPOINTMENT_H

#include <QWidget>

namespace Ui {
class Page_Appointment;
}

class PageAppointment : public QWidget {
    Q_OBJECT

public:
    explicit PageAppointment(QWidget *parent = nullptr);
    ~PageAppointment();

private:
    Ui::Page_Appointment *ui;
};

#endif // PAGE_APPOINTMENT_H
