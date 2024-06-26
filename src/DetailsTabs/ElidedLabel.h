/** @file
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ELIDED_LABEL_H
#define ELIDED_LABEL_H

#include <QLabel>

class ElidedLabel : public QLabel {
Q_OBJECT

public:
    explicit ElidedLabel(QWidget *parent = nullptr);

    void setSmallText(bool small_text = true) { small_text_ = small_text; }

protected:
    virtual bool event(QEvent *event);

    virtual void resizeEvent(QResizeEvent *);

private:
    bool small_text_;
    QString full_text_;

    void updateText();

Q_SIGNALS:

public Q_SLOTS:

    void clear();

    /**
     * @brief setText Set the label's plain text.
     * @param text The text to set. HTML will be escaped.
     */
    void setText(const QString &text);
};

#endif // ELIDED_LABEL_H