#ifndef PAGE_CHAT_H
#define PAGE_CHAT_H
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include "chatdatabase.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QScrollBar>
namespace Ui { class Page_Chat; }

class PageChat : public QWidget {
    Q_OBJECT
public:
    explicit PageChat(QWidget *parent = nullptr);
    ~PageChat();
    void loadHistory();
    void addMessageBubble(const ChatMessage &msg, bool isMine);
    void listenSocket(QTcpSocket *socket);
signals:
    void newMessage(const ChatMessage &msg);
private slots:
    void onSendClicked();
    void onMessageReceived(const QByteArray &data);
    void handleNewMessage(const ChatMessage &msg);
private:
    Ui::Page_Chat *ui;
    QVBoxLayout *chatLayout = nullptr;
    QTcpSocket *m_socket = nullptr;
    ChatDatabase db;
    int user_id = -1;
    QTimer *pollTimer = nullptr;
};
#endif // PAGE_CHAT_H
