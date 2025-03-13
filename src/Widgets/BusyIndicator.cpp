#include "BusyIndicator.h"
#include <QPainter>
#include <QStyle>
#include <QEvent>

BusyIndicator::BusyIndicator(QWidget *parent, QStyle::PixelMetric metric)
        : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_StyledBackground, false);

    hide();

    int size = style()->pixelMetric(metric);
    setFixedSize(size, size);

    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &BusyIndicator::updateAnimation);

    if (parent) {
        parent->installEventFilter(this);
    }
}

void BusyIndicator::start() {
    if (m_running)
        return;

    m_running = true;
    m_angle = 0;
    m_timer.start();
    setVisible(true);
    updatePosition();
    update();
}

void BusyIndicator::stop() {
    m_running = false;
    m_timer.stop();
    setVisible(false);
}

bool BusyIndicator::isRunning() const {
    return m_running;
}

void BusyIndicator::updateAnimation() {
    m_angle = (m_angle + 30) % 360;
    update();
}

void BusyIndicator::updatePosition()
{
    if (QWidget *parentWidget = qobject_cast<QWidget*>(parent())) {
        QSize vpSize = parentWidget->size();
        QSize indicatorSize = size();
        int x = (vpSize.width() - indicatorSize.width()) / 2;
        int y = (vpSize.height() - indicatorSize.height()) / 2;
        move(x, y);
    }
}

bool BusyIndicator::eventFilter(QObject *watched, QEvent *event) {
    if (watched == parent() && event->type() == QEvent::Resize) {
        updatePosition();
    }
    return QWidget::eventFilter(watched, event);
}

void BusyIndicator::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if (!m_running)
        return;

    if (width() <= 0 || height() <= 0)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor color = palette().color(QPalette::Text);

    int size = qMin(width(), height()) - 4;
    if (size <= 0) return;

    QPoint center(width() / 2, height() / 2);
    int radius = size / 2;
    if (radius <= 0) return;

    QPen pen(color, 2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 0; i < 12; ++i) {
        int alpha = static_cast<int>(255 * (i + 1) / 12.0);
        QColor segmentColor = color;
        segmentColor.setAlpha(alpha);

        painter.setPen(QPen(segmentColor, 2));
        painter.save();
        painter.translate(center);
        painter.rotate(m_angle - i * 30);

        int inner = qMax(0, radius / 2);
        int outer = qMax(0, radius - 2);

        painter.drawLine(0, -outer, 0, -inner);
        painter.restore();
    }
}
