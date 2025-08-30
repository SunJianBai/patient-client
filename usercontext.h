#ifndef USERCONTEXT_H
#define USERCONTEXT_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

class UserContext : public QObject {
    Q_OBJECT
public:
    static UserContext* instance() {
        static UserContext ctx;
        return &ctx;
    }
    int userId() const { return m_userId; }
    void setUserId(int id) { m_userId = id; }
    QString user() const { return m_user; }
    QString role() const { return m_role; }
    QString name() const { return m_name; }
    QString gender() const { return m_gender; }
    QString phone() const { return m_phone; }
    QString idNumber() const { return m_idNumber; }
    QString adress() const { return m_adress; }

    void setFromJson(const QJsonObject& obj) {
        m_user = obj.value("user").toString("未知用户");
        m_role = obj.value("role").toString("patient");
        m_name = obj.value("name").toString("未填写");
        m_gender = obj.value("gender").toString("未知");
        m_phone = obj.value("phone").toString("未填写");
        m_idNumber = obj.value("id_number").toString("未填写");
        m_adress = obj.value("adress").toString("未填写");
    }
private:
    explicit UserContext(QObject *parent = nullptr) : QObject(parent), m_userId(-1) {}
    int m_userId;
    QString m_user, m_role, m_name, m_gender, m_phone, m_idNumber, m_adress;
};

#endif // USERCONTEXT_H
