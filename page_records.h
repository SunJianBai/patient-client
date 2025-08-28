#ifndef PAGE_RECORDS_H
#define PAGE_RECORDS_H

#include <QWidget>

namespace Ui { class Page_Records; }

class PageRecords : public QWidget {
    Q_OBJECT

public:
    explicit PageRecords(QWidget *parent = nullptr);
    ~PageRecords();

private:
    Ui::Page_Records *ui;
};

#endif // PAGE_RECORDS_H
