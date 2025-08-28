#ifndef SIGNUPFORM_H
#define SIGNUPFORM_H

#include <QWidget>

namespace Ui {
class SignupForm;
}

class SignupForm : public QWidget
{
    Q_OBJECT
public:
    explicit SignupForm(QWidget *parent = nullptr, QWidget *loginWin = nullptr);
    ~SignupForm();

private slots:
    void on_signBack_clicked();
    void on_signSubmit_clicked();

private:
    Ui::SignupForm *ui;
    QWidget *loginWindow; // 新增成员
    QString username;
    QString password;
    QString repassword;
    QString name;
    QString gender;
    QString phone;
    QString number;
    QString area;
};

#endif // SIGNUPFORM_H
