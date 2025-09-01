#include "chatbubblewidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QTextDocument>
#include <QStyleOption>

ChatBubbleWidget::ChatBubbleWidget(const QString &text, const QString &time, BubbleType type, QWidget *parent)
    : QWidget(parent), m_text(text), m_time(time), m_type(type) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}


QSize ChatBubbleWidget::minimumSizeHint() const {
    QTextDocument doc;
    doc.setDefaultFont(font());
    QFontMetrics fm(font());
    int textPixelWidth = fm.horizontalAdvance(m_text);
    // 判断是否需要换行：如果文本一行且宽度小于最大宽度，则用实际宽度，否则用最大宽度
    int docTextWidth = (textPixelWidth < m_maxWidth && !m_text.contains("\n")) ? (textPixelWidth + 10) : m_maxWidth;
    doc.setTextWidth(docTextWidth);
    doc.setPlainText(m_text);
    QSizeF docSize = doc.size();
    int bubbleWidth = int(docSize.width()) + 2 * m_bubblePaddingH;
    int minBubbleWidth = 40 + 2 * m_bubblePaddingH;
    if (bubbleWidth < minBubbleWidth) bubbleWidth = minBubbleWidth;
    int bubbleHeight = int(docSize.height()) + 2 * m_bubblePaddingV;
    int totalHeight = bubbleHeight + m_timeHeight + 16;
    return QSize(bubbleWidth + 20, totalHeight);
}

QSize ChatBubbleWidget::sizeHint() const {
    return minimumSizeHint();
}

void ChatBubbleWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QTextDocument doc;
    doc.setDefaultFont(font());
    QFontMetrics fm(font());
    int textPixelWidth = fm.horizontalAdvance(m_text);
    int docTextWidth = (textPixelWidth < m_maxWidth && !m_text.contains("\n")) ? (textPixelWidth + 10) : m_maxWidth;
    doc.setTextWidth(docTextWidth);
    doc.setPlainText(m_text);
    QSizeF docSize = doc.size();
    int bubbleWidth = int(docSize.width()) + 2 * m_bubblePaddingH;
    int minBubbleWidth = 40 + 2 * m_bubblePaddingH;
    if (bubbleWidth < minBubbleWidth) bubbleWidth = minBubbleWidth;
    int bubbleHeight = int(docSize.height()) + 2 * m_bubblePaddingV;
    int x = (m_type == Mine) ? width() - bubbleWidth - 10 : 10;
    int y = 8;
    QRect bubbleRect(x, y, bubbleWidth, bubbleHeight);
    QColor bubbleColor = (m_type == Mine) ? QColor("#aee571") : QColor("#ffffff");
    QColor borderColor = (m_type == Mine) ? QColor("#aee571") : QColor("#d0d0d0");
    painter.setPen(borderColor);
    painter.setBrush(bubbleColor);
    painter.drawRoundedRect(bubbleRect, 12, 12);
    // 文本
    painter.save();
    painter.setPen(Qt::black);
    QRect textDrawRect = bubbleRect.adjusted(m_bubblePaddingH, m_bubblePaddingV, -m_bubblePaddingH, -m_bubblePaddingV);
    painter.translate(textDrawRect.topLeft());
    QRectF docRect(0, 0, textDrawRect.width(), textDrawRect.height());
    doc.drawContents(&painter, docRect);
    painter.restore();
    // 时间
    painter.setPen(QColor(120,120,120));
    painter.setFont(QFont(font().family(), 12));
    QRect timeRect(x, bubbleRect.bottom() + 4, bubbleWidth, m_timeHeight);
    painter.drawText(timeRect, Qt::AlignHCenter | Qt::AlignVCenter, m_time);
}
