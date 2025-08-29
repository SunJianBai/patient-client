#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H
#include <QFile>
#include <QDebug>
#include <QMainWindow>
#include "page_dashboard.h"
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"
#include "page_prescriptions.h"
#include "page_assessment.h"
#include "page_medicine.h"
#include "page_profile.h"

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

    // 子页面
    PageDashboard *pageDashboard;
    PageAppointment *pageAppointment;
    PageRecords *pageRecords;
    PagePrescriptions *pagePrescriptions;
    PageChat *pageChat;
    PageAssessment *pageAssessment;
    PageMedicine *pageMedicine;
    PageProfile *pageProfile;
};

#endif // MAIN_PAGE_H
