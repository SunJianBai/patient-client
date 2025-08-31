#ifndef PAGE_ASSESSMENT_H
#define PAGE_ASSESSMENT_H
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
#include <QJsonArray>
#include <QSslSocket>
#include <QProcess>
#include <QDir>
namespace Ui { class Page_Assessment; }
class PageAssessment : public QWidget {
    Q_OBJECT
public:
    explicit PageAssessment(QWidget *parent = nullptr);
    ~PageAssessment();
private slots:
    void onSubmitClicked();
private:
    Ui::Page_Assessment *ui;
    QStringList departmentList;
    QStringList doctorList;
};
#endif // PAGE_ASSESSMENT_H
