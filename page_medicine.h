#ifndef PAGE_MEDICINE_H
#define PAGE_MEDICINE_H
#include <QWidget>
namespace Ui { class Page_Medicine; }
class PageMedicine : public QWidget {
    Q_OBJECT
public:
    explicit PageMedicine(QWidget *parent = nullptr);
    ~PageMedicine();
private:
    Ui::Page_Medicine *ui;
};
#endif // PAGE_MEDICINE_H
