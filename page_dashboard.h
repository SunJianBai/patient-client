#ifndef PAGE_DASHBOARD_H
#define PAGE_DASHBOARD_H
#include <QWidget>
#include <QDebug>
namespace Ui { class Page_Dashboard; }
class PageDashboard : public QWidget {
    Q_OBJECT
public:
    explicit PageDashboard(QWidget *parent = nullptr);
    ~PageDashboard();
private:
    Ui::Page_Dashboard *ui;
};
#endif // PAGE_DASHBOARD_H
