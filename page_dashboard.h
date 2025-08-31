#ifndef PAGE_DASHBOARD_H
#define PAGE_DASHBOARD_H
#include <QWidget>
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

namespace Ui { class Page_Dashboard; }
class PageDashboard : public QWidget {
    Q_OBJECT
public:
    explicit PageDashboard(QWidget *parent = nullptr);
    ~PageDashboard();
    void fetchAppointments();
    void fetchHealthResult();
private:
    Ui::Page_Dashboard *ui;
};
#endif // PAGE_DASHBOARD_H
