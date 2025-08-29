#ifndef PAGE_PRESCRIPTIONS_H
#define PAGE_PRESCRIPTIONS_H
#include <QWidget>
namespace Ui { class Page_Prescriptions; }
class PagePrescriptions : public QWidget {
    Q_OBJECT
public:
    explicit PagePrescriptions(QWidget *parent = nullptr);
    ~PagePrescriptions();
private:
    Ui::Page_Prescriptions *ui;
};
#endif // PAGE_PRESCRIPTIONS_H
