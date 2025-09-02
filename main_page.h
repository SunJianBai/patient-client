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
#include <QJsonArray>

#include "page_dashboard.h"
#include "page_appointment.h"
#include "page_records.h"
#include "page_chat.h"
#include "page_assessment.h"
#include "page_profile.h"
#include "usercontext.h"

namespace Ui {
class Main_Page;
}




class Main_Page : public QMainWindow
{
    Q_OBJECT
public:
    explicit Main_Page(QWidget *parent = nullptr);
    ~Main_Page();
    void setSocket(QTcpSocket *socket);
    QTcpSocket *m_socket = nullptr;
private:
    Ui::Main_Page *ui;
    // 子页面延迟初始化
    PageDashboard *pageDashboard = nullptr;
    PageAppointment *pageAppointment = nullptr;
    PageRecords *pageRecords = nullptr;
    PageChat *pageChat = nullptr;
    PageAssessment *pageAssessment = nullptr;
    PageProfile *pageProfile = nullptr;
};

#endif // MAIN_PAGE_H
