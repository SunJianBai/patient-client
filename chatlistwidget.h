#pragma once
#include <QWidget>
#include <QListWidget>
#include <QDateTime>
#include "chatdatabase.h"

class QNChatMessage;

class ChatListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit ChatListWidget(QWidget *parent = nullptr);
    void addChatMessage(const ChatMessage &msg, bool isMine);
    void addTimeMessage(const QString &timeStr);
    void reloadHistory(const QList<ChatMessage> &msgs, int selfUserId);
};
