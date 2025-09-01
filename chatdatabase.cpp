#include "chatdatabase.h"
#include <QSqlError>
#include <QDir>

ChatDatabase::ChatDatabase(QObject *parent) : QObject(parent) {}

ChatDatabase::~ChatDatabase() { close(); }

bool ChatDatabase::open(int user_id) {
    if (db.isOpen() && current_user_id == user_id) {
        qDebug() << "[ChatDB] 数据库已打开，user_id=" << user_id;
        return true;
    }
    close();
    QString dbPath = QDir::homePath() + QString("/.patient_chat_%1.db").arg(user_id);
    db = QSqlDatabase::addDatabase("QSQLITE", QString("chatdb_%1").arg(user_id));
    db.setDatabaseName(dbPath);
    qDebug() << "[ChatDB] 尝试打开数据库:" << dbPath;
    if (!db.open()) {
        qDebug() << "[ChatDB] 打开数据库失败:" << db.lastError().text();
        return false;
    }
    current_user_id = user_id;
    QSqlQuery query(db);
    QString createTable = "CREATE TABLE IF NOT EXISTS messages ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "user_id INTEGER,"
                         "sender TEXT,"
                         "content TEXT,"
                         "time TEXT"
                         ")";
    if (!query.exec(createTable)) {
        qDebug() << "[ChatDB] 创建表失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "[ChatDB] 数据库打开并表已准备好, user_id=" << user_id;
    return true;
}

void ChatDatabase::close() {
    if (db.isOpen()) {
        qDebug() << "[ChatDB] 关闭数据库, user_id=" << current_user_id;
        db.close();
    }
    current_user_id = -1;
}

bool ChatDatabase::addMessage(const ChatMessage &msg) {
    if (!db.isOpen()) {
        qDebug() << "[ChatDB] 数据库未打开，无法插入消息";
        return false;
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO messages (user_id, sender, content, time) VALUES (?, ?, ?, ?)");
    query.addBindValue(msg.user_id);
    query.addBindValue(msg.sender);
    query.addBindValue(msg.content);
    query.addBindValue(msg.time);
    qDebug() << "[ChatDB] 插入消息:" << msg.user_id << msg.sender << msg.content << msg.time;
    bool ok = query.exec();
    if (!ok) qDebug() << "[ChatDB] 插入消息失败:" << query.lastError().text();
    return ok;
}

QList<ChatMessage> ChatDatabase::getMessages(int user_id) {
    QList<ChatMessage> list;
    if (!db.isOpen()) {
        qDebug() << "[ChatDB] 数据库未打开，无法查询消息";
        return list;
    }
    QSqlQuery query(db);
    query.prepare("SELECT id, user_id, sender, content, time FROM messages WHERE user_id=? ORDER BY time ASC");
    query.addBindValue(user_id);
    qDebug() << "[ChatDB] 查询消息，user_id=" << user_id;
    if (!query.exec()) {
        qDebug() << "[ChatDB] 查询消息失败:" << query.lastError().text();
        return list;
    }
    while (query.next()) {
        ChatMessage msg;
        msg.id = query.value(0).toInt();
        msg.user_id = query.value(1).toInt();
        msg.sender = query.value(2).toString();
        msg.content = query.value(3).toString();
        msg.time = query.value(4).toString();
        list.append(msg);
        qDebug() << "[ChatDB] 读取消息:" << msg.id << msg.user_id << msg.sender << msg.content << msg.time;
    }
    return list;
}
