#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H
#include <QMainWindow>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>


#include "page_dashboard.h"
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"
#include "page_assessment.h"
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
    QTcpSocket *m_socket = nullptr; // 新增socket成员
    // 子页面
    PageDashboard *pageDashboard;
    PageAppointment *pageAppointment;
    PageRecords *pageRecords;
    PageChat *pageChat;
    PageAssessment *pageAssessment;
    PageProfile *pageProfile;
};

#endif // MAIN_PAGE_H
