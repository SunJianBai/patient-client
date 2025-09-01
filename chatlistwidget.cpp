#include "chatlistwidget.h"
#include "qnchatmessage.h"
#include <QListWidgetItem>
#include <QDateTime>

ChatListWidget::ChatListWidget(QWidget *parent) : QListWidget(parent) {
    setStyleSheet("QListWidget{border:none;background:#f5f5f5;}");
    setSpacing(8);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void ChatListWidget::addChatMessage(const ChatMessage &msg, bool isMine) {
    QString time = QString::number(QDateTime::fromString(msg.time, "yyyy-MM-dd HH:mm:ss").toTime_t());
    QNChatMessage::User_Type type = isMine ? QNChatMessage::User_Me : QNChatMessage::User_She;
    QNChatMessage* messageW = new QNChatMessage(this);
    QListWidgetItem* item = new QListWidgetItem(this);
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(msg.content);
    item->setSizeHint(size);
    messageW->setText(msg.content, time, size, type);
    setItemWidget(item, messageW);
}

void ChatListWidget::addTimeMessage(const QString &timeStr) {
    QNChatMessage* messageTime = new QNChatMessage(this);
    QListWidgetItem* itemTime = new QListWidgetItem(this);
    QSize size = QSize(this->width(), 40);
    messageTime->resize(size);
    itemTime->setSizeHint(size);
    messageTime->setText(timeStr, timeStr, size, QNChatMessage::User_Time);
    setItemWidget(itemTime, messageTime);
}

void ChatListWidget::reloadHistory(const QList<ChatMessage> &msgs, int selfUserId) {
    clear();
    QString lastTime;
    for (const ChatMessage &msg : msgs) {
        QString curTime = msg.time;
        if (lastTime.isEmpty() || QDateTime::fromString(curTime, "yyyy-MM-dd HH:mm:ss").toTime_t() - QDateTime::fromString(lastTime, "yyyy-MM-dd HH:mm:ss").toTime_t() > 60) {
            addTimeMessage(curTime);
        }
        addChatMessage(msg, msg.sender == "patient");
        lastTime = curTime;
    }
}
