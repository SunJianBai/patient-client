#ifndef PAGE_PROFILE_H
#define PAGE_PROFILE_H
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
#include <QApplication>
#include <QJsonArray>
namespace Ui { class Page_Profile; }
class PageProfile : public QWidget {
    Q_OBJECT
public:
    explicit PageProfile(QWidget *parent = nullptr);
    ~PageProfile();
    Q_INVOKABLE void refreshUserInfo();
private:
    Ui::Page_Profile *ui;
};
#endif // PAGE_PROFILE_H
