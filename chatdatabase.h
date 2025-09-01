#ifndef CHATDATABASE_H
#define CHATDATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QDebug>

struct ChatMessage {
    int id;
    int user_id;
    QString sender;
    QString content;
    QString time;
};

class ChatDatabase : public QObject {
    Q_OBJECT
public:
    explicit ChatDatabase(QObject *parent = nullptr);
    ~ChatDatabase();
    bool open(int user_id);
    void close();
    bool addMessage(const ChatMessage &msg);
    QList<ChatMessage> getMessages(int user_id);
private:
    QSqlDatabase db;
    int current_user_id = -1;
};

#endif // CHATDATABASE_H
