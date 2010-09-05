/*
 *   Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 *   Copyright (C) 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ApplicationDelegate.h"

// Qt includes
#include <QApplication>
#include <QtGui/QAbstractItemView>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

// KDE includes
#include <KDebug>
#include <KIconLoader>
#include <KGlobal>
#include <KLocale>
#include <KStandardDirs>
#include <Nepomuk/KRatingPainter>

// LibQApt includes
#include <LibQApt/Package>

#include "ApplicationModel.h"

#define FAV_ICON_SIZE 24
#define EMBLEM_ICON_SIZE 8
#define UNIVERSAL_PADDING 4
#define FADE_LENGTH 16
#define MAIN_ICON_SIZE 32

ApplicationDelegate::ApplicationDelegate(QAbstractItemView *parent)
  : KExtendableItemDelegate(parent),
    // loads it here to be faster when displaying items
    m_installIcon("download"),
    m_installString(i18n("Install")),
    m_removeIcon("edit-delete"),
    m_removeString(i18n("Remove")),
    m_unknownIcon("applications-other")
{
    // To get sizing.
    QPushButton button, button2;
    button.setText(m_installString);
    button.setIcon(m_installIcon);
    button2.setText(m_removeString);
    button2.setIcon(m_removeIcon);
    m_buttonSize = button.sizeHint();
    int width = qMax(button.sizeHint().width(), button2.sizeHint().width());
    width = qMax(width, button2.sizeHint().width());
    m_buttonSize.setWidth(width);
    m_buttonIconSize = button.iconSize();

    // For icons later
    KGlobal::dirs()->addResourceDir("appicon", "/usr/share/app-install/icons/");
}

void ApplicationDelegate::paint(QPainter *painter,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    bool leftToRight = (painter->layoutDirection() == Qt::LeftToRight);

    QStyleOptionViewItemV4 opt(option);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    painter->save();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    painter->restore();

    // pain the background (checkbox and the extender)
    KExtendableItemDelegate::paint(painter, opt, index);

    int leftCount;
    if (leftToRight) {
        opt.rect.setLeft(option.rect.left() + UNIVERSAL_PADDING);
        leftCount = opt.rect.left() + UNIVERSAL_PADDING;
    } else {
        opt.rect.setRight(option.rect.right() - UNIVERSAL_PADDING);
        leftCount = opt.rect.width() - (UNIVERSAL_PADDING + MAIN_ICON_SIZE);
    }

    int left = opt.rect.left();
    int top = opt.rect.top();
    int width = opt.rect.width();

    KRatingPainter ratingPainter;
    QRect rect = opt.rect;

    if (leftToRight) {
        rect.setLeft(left + width - (m_buttonSize.width() + UNIVERSAL_PADDING));
        width -= m_buttonSize.width() + UNIVERSAL_PADDING;
    } else {
        rect.setLeft(left + UNIVERSAL_PADDING);
        left += m_buttonSize.width() + UNIVERSAL_PADDING;
    }
    // Calculate the top of the ratings widget which is the item height - the widget height size divided by 2
    // this give us a little value which is the top and bottom margin
    rect.setTop(rect.top() + ((calcItemHeight(option) - m_buttonSize.height()) / 2));
    rect.setSize(m_buttonSize); // the width and height sizes of the button

    ratingPainter.paint(painter, rect, index.data(ApplicationModel::PopconRole).toInt());


    // selects the mode to paint the icon based on the info field
    QIcon::Mode iconMode = QIcon::Normal;
    if (option.state & QStyle::State_MouseOver) {
        iconMode = QIcon::Active;
    }

    QColor foregroundColor = (option.state.testFlag(QStyle::State_Selected))?
    option.palette.color(QPalette::HighlightedText):option.palette.color(QPalette::Text);

    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_normal.font.setPointSize(local_option_normal.font.pointSize() - 1);

    QPixmap pixmap(option.rect.size());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.translate(-option.rect.topLeft());

    // Main icon
    KIcon icon(KIconLoader::global()->loadIcon(index.data(ApplicationModel::IconRole).toString(),
                                                       KIconLoader::User, 0, KIconLoader::DefaultState,
                                                       QStringList(), 0L, true));
    if (icon.isNull()) {
        icon = m_unknownIcon;
    }

    int iconSize = calcItemHeight(option) - 2 * UNIVERSAL_PADDING;
    icon.paint(&p,
               leftCount,
               top + UNIVERSAL_PADDING,
               iconSize,
               iconSize,
               Qt::AlignCenter,
               iconMode);

    int textWidth;
    if (leftToRight) {
        // add the main icon
        leftCount += iconSize + UNIVERSAL_PADDING;
        textWidth = width - (leftCount - left);
    } else {
        leftCount -= UNIVERSAL_PADDING;
        textWidth = leftCount - left;
        leftCount = left;
    }

    // Text
    const int itemHeight = calcItemHeight(option);

    p.setPen(foregroundColor);
    // draw the top line
    int topTextHeight = QFontInfo(local_option_title.font).pixelSize();
    p.setFont(local_option_title.font);
    p.drawText(leftCount,
               top,
               textWidth,
               topTextHeight + UNIVERSAL_PADDING,
               Qt::AlignVCenter | Qt::AlignLeft,
               index.data(ApplicationModel::NameRole).toString());

    // draw the bottom line
    iconSize = topTextHeight + UNIVERSAL_PADDING;

    // store the original opacity
    qreal opa = p.opacity();
    if (!(option.state & QStyle::State_MouseOver) && !(option.state & QStyle::State_Selected)) {
        p.setOpacity(opa / 2.5);
    }

    p.setFont(local_option_normal.font);
    p.drawText(leftToRight ? leftCount + 0.5*iconSize: left - UNIVERSAL_PADDING,
               top + itemHeight / 2,
               textWidth - iconSize,
               QFontInfo(local_option_normal.font).pixelSize() + UNIVERSAL_PADDING,
               Qt::AlignTop | Qt::AlignLeft,
               index.data(ApplicationModel::CommentRole).toString());
    p.setOpacity(opa);

    painter->drawPixmap(option.rect.topLeft(), pixmap);
}

int ApplicationDelegate::calcItemHeight(const QStyleOptionViewItem &option) const
{
    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_normal.font.setPointSize(local_option_normal.font.pointSize() - 1);

    int textHeight = QFontInfo(local_option_title.font).pixelSize() + QFontInfo(local_option_normal.font).pixelSize();
    return textHeight + 3 * UNIVERSAL_PADDING;
}

bool ApplicationDelegate::editorEvent(QEvent *event,
                                    QAbstractItemModel *model,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index)
{
    Q_UNUSED(option)
    if (event->type() == QEvent::MouseButtonRelease) {
       itemActivated(index);
    }

    return KExtendableItemDelegate::editorEvent(event, model, option, index);
}

QSize ApplicationDelegate::sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index ) const
{
    int width = (index.column() == 0) ? index.data(Qt::SizeHintRole).toSize().width() : FAV_ICON_SIZE + 2 * UNIVERSAL_PADDING;
    QSize ret(KExtendableItemDelegate::sizeHint(option, index));
    // remove the default size of the index
    ret -= QStyledItemDelegate::sizeHint(option, index);

    ret.rheight() += calcItemHeight(option);
    ret.rwidth()  += width;

    return ret;
}

void ApplicationDelegate::itemActivated(QModelIndex index)
{
    if (index == m_oldIndex) {
        return;
    }

    if (isExtended(m_oldIndex)) {
        contractItem(m_oldIndex);
    }

    Application *app = static_cast<const ApplicationModel*>(index.model())->applicationAt(index);

    QTreeView *view = static_cast<QTreeView*>(parent());
    m_extender = new QWidget(view);

    QHBoxLayout *layout = new QHBoxLayout(m_extender);
    m_extender->setLayout(layout);

    QPushButton *infoButton = new QPushButton(m_extender);
    infoButton->setText(i18n("More Info"));
    layout->addWidget(infoButton);

    QWidget *buttonSpacer = new QWidget(m_extender);
    buttonSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    layout->addWidget(buttonSpacer);

    QPushButton *actionButton = new QPushButton(m_extender);

    int state = app->package()->state();
    if (state & QApt::Package::Installed) {
        actionButton->setIcon(m_removeIcon);
        actionButton->setText(m_removeString);
    } else {
        actionButton->setIcon(m_installIcon);
        actionButton->setText(m_installString);
    }
    layout->addWidget(actionButton);

    extendItem(m_extender, index);
    m_oldIndex = index;
}

#include "ApplicationDelegate.moc"
