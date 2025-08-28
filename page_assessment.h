#ifndef PAGE_ASSESSMENT_H
#define PAGE_ASSESSMENT_H
#include <QWidget>
namespace Ui { class Page_Assessment; }
class PageAssessment : public QWidget {
    Q_OBJECT
public:
    explicit PageAssessment(QWidget *parent = nullptr);
    ~PageAssessment();
private:
    Ui::Page_Assessment *ui;
};
#endif // PAGE_ASSESSMENT_H
