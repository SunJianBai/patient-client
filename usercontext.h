#ifndef USERCONTEXT_H
#define USERCONTEXT_H

#include <QObject>

class UserContext : public QObject {
    Q_OBJECT
public:
    static UserContext* instance() {
        static UserContext ctx;
        return &ctx;
    }
    int userId() const { return m_userId; }
    void setUserId(int id) { m_userId = id; }
    // 可扩展更多用户信息
private:
    explicit UserContext(QObject *parent = nullptr) : QObject(parent), m_userId(-1) {}
    int m_userId;
};

#endif // USERCONTEXT_H
