#ifndef PAGE_APPOINTMENT_H
#define PAGE_APPOINTMENT_H

#include <QWidget>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>
#include <QApplication>

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
