#ifndef BUSYINDICATOR_H
#define BUSYINDICATOR_H

#include <QWidget>
#include <QTimer>
#include <QStyle>

class BusyIndicator : public QWidget {
Q_OBJECT

public:
    explicit BusyIndicator(QWidget *parent = nullptr, QStyle::PixelMetric metric = QStyle::PM_LargeIconSize);

    void start();

    void stop();

    bool isRunning() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private Q_SLOTS:

private:
    void updateAnimation();
    void updatePosition();

private:
    QTimer m_timer;
    int m_angle = 0;
    bool m_running = false;
};

#endif
