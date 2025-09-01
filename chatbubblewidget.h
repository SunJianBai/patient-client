#ifndef CHATBUBBLEWIDGET_H
#define CHATBUBBLEWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QFontMetrics>
#include <QTextOption>

class ChatBubbleWidget : public QWidget {
    Q_OBJECT
public:
    enum BubbleType { Mine, Other };
    explicit ChatBubbleWidget(const QString &text, const QString &time, BubbleType type, QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QString m_text;
    QString m_time;
    BubbleType m_type;
    int m_maxWidth = 500;
    int m_bubblePaddingH = 16; // 气泡左右padding
    int m_bubblePaddingV = 10; // 气泡上下padding
    int m_timeHeight = 20;     // 时间高度
};

#endif // CHATBUBBLEWIDGET_H
