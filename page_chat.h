#ifndef PAGE_CHAT_H
#define PAGE_CHAT_H
#include <QWidget>
namespace Ui { class Page_Chat; }
class PageChat : public QWidget {
    Q_OBJECT
public:
    explicit PageChat(QWidget *parent = nullptr);
    ~PageChat();
private:
    Ui::Page_Chat *ui;
};
#endif // PAGE_CHAT_H
