#ifndef PAGE_PROFILE_H
#define PAGE_PROFILE_H
#include <QWidget>
namespace Ui { class Page_Profile; }
class PageProfile : public QWidget {
    Q_OBJECT
public:
    explicit PageProfile(QWidget *parent = nullptr);
    ~PageProfile();
private:
    Ui::Page_Profile *ui;
};
#endif // PAGE_PROFILE_H
