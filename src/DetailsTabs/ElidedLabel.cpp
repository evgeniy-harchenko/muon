/* elided_label.cpp
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ElidedLabel.h"

#include <QFontMetrics>
#include <QResizeEvent>

ElidedLabel::ElidedLabel(QWidget *parent) :
        QLabel(parent),
        small_text_(false) {
    QFontMetrics fm(font());
    setMinimumWidth(fm.height() * 5); // em-widths
}

bool ElidedLabel::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::ApplicationPaletteChange:
            updateText();
            break;
        default:
            break;

    }
    return QLabel::event(event);
}

void ElidedLabel::resizeEvent(QResizeEvent *) {
    updateText();
}

void ElidedLabel::updateText() {
    double fudged_width = small_text_ ? width() * 1.2 : width();
    QString elided_text = fontMetrics().elidedText(full_text_, Qt::ElideRight, (int)fudged_width);
    QString label_text = small_text_ ? QStringLiteral("<small><i>") : QStringLiteral("<i>"); // TODO: go away from <i>

    label_text += elided_text;
    label_text += small_text_ ? QStringLiteral("</i></small> ") : QStringLiteral("</i> ");
    QLabel::setText(label_text);
}

void ElidedLabel::clear() {
    full_text_.clear();
    setToolTip(QStringLiteral(""));
    updateText();
}

void ElidedLabel::setText(const QString &text) {
    full_text_ = text.toHtmlEscaped();
    updateText();
}