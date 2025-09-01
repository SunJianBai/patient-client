#include "page_chat.h"
#include "ui_page_chat.h"

#include "usercontext.h"
#include "main_page.h"
#include "chatbubblewidget.h"

class ChatBubble : public QLabel {
public:
    explicit ChatBubble(const QString &text, QWidget *parent=nullptr)
        : QLabel(text, parent) {
        setWordWrap(true);
        setTextInteractionFlags(Qt::TextSelectableByMouse);

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    }

    QSize sizeHint() const override {
        int maxWidth = 500;
        QFontMetrics fm(font());

        // 计算在限定宽度下的排版结果
        QRect rect = fm.boundingRect(QRect(0, 0, maxWidth, 10000),
                                     Qt::TextWordWrap | Qt::AlignLeft,
                                     text());

        int realWidth = rect.width();
        int realHeight = rect.height();

        // ⚡ 宽度 = min(实际文本宽度, maxWidth)，保证短文本不会被拉长
        int finalWidth = qMin(realWidth, maxWidth) + 20;  // +padding
        return QSize(finalWidth, realHeight + 20);        // 高度跟随内容
    }
};

PageChat::PageChat(QWidget *parent) : QWidget(parent), ui(new Ui::Page_Chat) {
    ui->setupUi(this);
    QFile qssFile(":/style/styles/chat_style.qss");
    if (qssFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(style);
        qssFile.close();
    }
    chatLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    user_id = UserContext::instance()->userId();
    db.open(user_id);
    qDebug() << "[Chat] 初始化 PageChat, user_id=" << user_id;

    connect(ui->btn_send, &QPushButton::clicked, this, &PageChat::onSendClicked);
    connect(this, &PageChat::newMessage, this, &PageChat::handleNewMessage);

    // 自动查找主页面socket并监听，保证始终监听服务器消息
    QTcpSocket *socket = nullptr;
    QWidget *p = parentWidget();
    while (p) {
        Main_Page *mainPage = qobject_cast<Main_Page *>(p);
        if (mainPage) { socket = mainPage->m_socket; break; }
        p = p->parentWidget();
    }
    if (socket) {
        qDebug() << "[Chat] 构造时自动调用listenSocket, socket=" << socket;
        listenSocket(socket);
    } else {
        qDebug() << "[Chat] 构造时未找到主页面socket, 无法自动监听";
    }
    // 切换到本页面时自动加载历史
    loadHistory();
}

PageChat::~PageChat() {
    db.close();
    delete ui;
}

void PageChat::loadHistory() {
    qDebug() << "[Chat] 加载本地历史消息";
    if (!chatLayout) return;
    // 清空原有气泡
    QLayoutItem *child;
    while ((child = chatLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    QList<ChatMessage> msgs = db.getMessages(user_id);
    for (const ChatMessage &msg : msgs) {
        bool isMine = (msg.sender == "patient");
        addMessageBubble(msg, isMine);
    }
    // 滚动到底部
    QTimer::singleShot(100, [this]{
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    });
}

void PageChat::addMessageBubble(const ChatMessage &msg, bool isMine) {
    // 先插入时间气泡（居中，仿微信）
    if (!msg.time.isEmpty()) {
        QLabel *timeLabel = new QLabel(msg.time, this);
        timeLabel->setAlignment(Qt::AlignHCenter);
        timeLabel->setStyleSheet("color:gray;font-size:24px;padding:4px 16px;background:rgba(220,220,220,0.7);border-radius:8px;margin:8px 0;");
        chatLayout->addWidget(timeLabel, 0, Qt::AlignHCenter);
    }
    // 聊天气泡
    ChatBubbleWidget *bubble = new ChatBubbleWidget(
        msg.content, "", isMine ? ChatBubbleWidget::Mine : ChatBubbleWidget::Other, this);
    chatLayout->addWidget(bubble);
    // 滚动到底部
    QTimer::singleShot(100, [this]{
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    });
}

void PageChat::onSendClicked() {
    QString text = ui->textEdit_input->toPlainText().trimmed();
    if (text.isEmpty()) return;
    QTcpSocket *socket = nullptr;
    QWidget *p = parentWidget();
    while (p) {
        Main_Page *mainPage = qobject_cast<Main_Page *>(p);
        if (mainPage) { socket = mainPage->m_socket; break; }
        p = p->parentWidget();
    }
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "[Chat] 发送失败，socket未连接";
        return;
    }
    QJsonObject req;
    req["type"] = "message";
    req["sender"] = "patient";
    req["content"] = text;
    QJsonDocument doc(req);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream << static_cast<quint32>(jsonData.size());
    packet.append(jsonData);
    qDebug() << "[Chat] 发送消息:" << QString::fromUtf8(jsonData);
    socket->write(packet);
    socket->flush();
    // 本地先显示
    ChatMessage msg;
    msg.user_id = user_id;
    msg.sender = "patient";
    msg.content = text;
    msg.time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    db.addMessage(msg);
    emit newMessage(msg);
    ui->textEdit_input->clear();
}

void PageChat::listenSocket(QTcpSocket *socket) {
    m_socket = socket;
    if (!m_socket) {
        qDebug() << "[Chat] listenSocket: socket为空，无法监听";
        return;
    }
    qDebug() << "[Chat] listenSocket: 开始监听服务器消息, socket=" << m_socket;
    connect(m_socket, &QTcpSocket::readyRead, this, [=](){
        while (m_socket->bytesAvailable() > 0) {
            QByteArray resp = m_socket->readAll();
            qDebug() << "[Chat] 收到服务器消息，长度:" << resp.size();
            if (resp.size() >= 4) {
                QDataStream respStream(resp);
                respStream.setVersion(QDataStream::Qt_5_10);
                quint32 len = 0;
                respStream >> len;
                QByteArray jsonResp = resp.right(resp.size() - 4);
                qDebug() << "[Chat] 服务器返回json:" << QString::fromUtf8(jsonResp);
                QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
                if (respDoc.isObject()) {
                    QJsonObject respObj = respDoc.object();
                    if (respObj.value("type").toString() == "message.return") {
                        QString sender = respObj.value("sender").toString();
                        QString content = respObj.value("content").toString();
                        QString time = respObj.value("time").toString();
                        // 过滤掉自己刚发的（本地已显示）
                        if (sender == "patient") {
                            qDebug() << "[Chat] 收到自己发出的消息，忽略";
                            return;
                        }
                        ChatMessage msg;
                        msg.user_id = user_id;
                        msg.sender = sender;
                        msg.content = content;
                        msg.time = time;
                        bool ok = db.addMessage(msg);
                        qDebug() << "[Chat] 新消息已存入本地数据库, ok=" << ok << ", sender=" << sender << ", content=" << content << ", time=" << time;
                        emit newMessage(msg);
                    } else {
                        qDebug() << "[Chat] 收到非聊天消息，type=" << respObj.value("type").toString();
                    }
                } else {
                    qDebug() << "[Chat] 服务器返回非对象JSON，忽略";
                }
            } else {
                qDebug() << "[Chat] 服务器返回数据长度异常，忽略";
            }
        }
    }, Qt::UniqueConnection);
}

void PageChat::handleNewMessage(const ChatMessage &msg) {
    Q_UNUSED(msg);
    loadHistory();
}

// 修复未实现的onMessageReceived槽，防止链接错误
void PageChat::onMessageReceived(const QByteArray &data) {
    Q_UNUSED(data);
    qDebug() << "[Chat] onMessageReceived 槽被调用（未实现，忽略）";
}
