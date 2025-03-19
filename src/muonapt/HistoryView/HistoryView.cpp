/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "HistoryView.h"

#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QListView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QStandardItemModel>
#include <QEvent>
#include <QHeaderView>

#include <QApplication>
#include <KLocalizedString>

#include <QApt/History>

#include "HistoryProxyModel.h"

const QString itemStyleSheet = QStringLiteral("QTreeView::item { padding-left: 10px; padding-right: 10px; }");

HistoryView::HistoryView(QWidget *parent)
    : QWidget(parent),
      m_colorScheme(QPalette::Current, KColorScheme::Window)
{
    QLayout *viewLayout = new QVBoxLayout(this);
    setLayout(viewLayout);
    m_history = new QApt::History(this);

    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    QLabel *headerLabel = new QLabel(headerWidget);
    headerLabel->setText(xi18nc("@info", "<title>History</title>"));

    QWidget *headerSpacer = new QWidget(headerWidget);
    headerSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_searchEdit = new QLineEdit(headerWidget);
    m_searchEdit->setPlaceholderText(i18nc("@label Line edit click message", "Search"));
    m_searchEdit->setClearButtonEnabled(true);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setInterval(300);
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this, &HistoryView::startSearch);
    connect(m_searchEdit, &QLineEdit::textChanged, m_searchTimer, [this](){ m_searchTimer->start(); });

    QIcon installIcon = QIcon::fromTheme(QStringLiteral("download"));
    QIcon upgradeIcon = QIcon::fromTheme(QStringLiteral("system-software-update"));
    QIcon removeIcon = QIcon::fromTheme(QStringLiteral("edit-delete"));
    QIcon downgradeIcon = QIcon::fromTheme(QStringLiteral("go-down"));
    QIcon purgeIcon = QIcon::fromTheme(QStringLiteral("edit-delete-shred"));
    QIcon reinstallIcon = QIcon::fromTheme(QStringLiteral("view-refresh"));

    m_filterBox = new QComboBox(headerWidget);
    m_filterBox->insertItem(AllChangesItem, QIcon::fromTheme(QStringLiteral("bookmark-new-list")),
                            i18nc("@item:inlistbox Filters all changes in the history view",
                                  "All changes"),
                            0);
    m_filterBox->insertItem(InstallationsItem, installIcon,
                            i18nc("@item:inlistbox Filters installations in the history view",
                                  "Installations"),
                            QApt::Package::ToInstall);
    m_filterBox->insertItem(UpdatesItem, upgradeIcon,
                            i18nc("@item:inlistbox Filters updates in the history view",
                                  "Updates"),
                            QApt::Package::ToUpgrade);
    m_filterBox->insertItem(RemovalsItem, removeIcon,
                            i18nc("@item:inlistbox Filters removals in the history view",
                                  "Removals"),
                            (QApt::Package::State)(QApt::Package::ToRemove | QApt::Package::ToPurge));
    m_filterBox->insertItem(ReinstallationsItem, reinstallIcon,
                            i18nc("@item:inlistbox Filters removals in the history view",
                                  "Reinstallations"),
                            QApt::Package::ToReInstall);
    connect(m_filterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setStateFilter(int)));

    headerLayout->addWidget(headerLabel);
    headerLayout->addWidget(headerSpacer);
    headerLayout->addWidget(m_searchEdit);
    headerLayout->addWidget(m_filterBox);

    viewLayout->addWidget(headerWidget);

    m_historyModel = new QStandardItemModel(this);
    m_historyModel->setColumnCount(3);
    m_historyModel->setHeaderData(0, Qt::Horizontal, i18nc("@title:column", "Package"));
    m_historyModel->setHeaderData(1, Qt::Horizontal, i18nc("@title:column", "Action"));
    m_historyModel->setHeaderData(2, Qt::Horizontal, i18nc("@title:column", "Time"));

    m_historyView = new QTreeView(this);
    m_historyView->setRootIsDecorated(true);
    m_historyView->setAlternatingRowColors(true);
    m_historyView->setMouseTracking(true);
    m_historyView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_historyView->setStyleSheet(itemStyleSheet);
    m_historyView->header()->setStretchLastSection(false);

    QHash<QString, QString> categoryHash;

    QHash<QApt::Package::State, QString> actionHash;
    actionHash[QApt::Package::ToInstall] = i18nc("@info:status describes a past-tense action", "Installed");
    actionHash[QApt::Package::ToUpgrade] = i18nc("@info:status describes a past-tense action", "Upgraded");
    actionHash[QApt::Package::ToDowngrade] = i18nc("@status describes a past-tense action", "Downgraded");
    actionHash[QApt::Package::ToRemove] = i18nc("@status describes a past-tense action", "Removed");
    actionHash[QApt::Package::ToPurge] = i18nc("@status describes a past-tense action", "Purged");
    actionHash[QApt::Package::ToReInstall] = i18nc("@status describes a past-tense action", "Reinstalled");

    for (const QApt::HistoryItem &item: m_history->historyItems()) {
        QLocale locale;
        QDateTime startDateTime = item.startDate();
        QString formattedTime = locale.toString(startDateTime, QLocale::LongFormat);
        QString category;

        QString date = startDateTime.date().toString();
        if (categoryHash.contains(date)) {
            category = categoryHash.value(date);
        } else {
            category = locale.toString(startDateTime.date(), QLocale::ShortFormat);
            categoryHash[date] = category;
        }

        QStandardItem *parentItem = nullptr;
        if (!m_dateCategoryItems.contains(category)) {
            QList<QStandardItem*> parentRow;
            QStandardItem *dateItem = new QStandardItem(category);
            dateItem->setEditable(false);
            dateItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            parentRow << dateItem;
            m_historyModel->appendRow(parentRow);
            m_dateCategoryItems[category] = dateItem;
            parentItem = dateItem;
        } else {
            parentItem = m_dateCategoryItems.value(category);
        }

        auto addChildRow = [&](const QString &pkg, QApt::Package::State pastAction, const QIcon &icon) {
            QList<QStandardItem*> childRow;
            QStandardItem *pkgItem = new QStandardItem(icon, pkg);
            pkgItem->setEditable(false);
            pkgItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            pkgItem->setData(pastAction, HistoryProxyModel::HistoryActionRole);

            QStandardItem *actionItem = new QStandardItem(actionHash.value(pastAction));
            actionItem->setEditable(false);
            actionItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            actionItem->setData(pastAction, HistoryProxyModel::HistoryActionRole);

            QStandardItem *timeItem = new QStandardItem(formattedTime);
            timeItem->setEditable(false);
            timeItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            timeItem->setData(pastAction, HistoryProxyModel::HistoryActionRole);

            updateItemColors(pkgItem, m_colorScheme);
            updateItemColors(actionItem, m_colorScheme);
            updateItemColors(timeItem, m_colorScheme);

            childRow << pkgItem << actionItem << timeItem;
            parentItem->appendRow(childRow);
        };

        for (const QString &package: item.installedPackages()) {
            addChildRow(package, QApt::Package::ToInstall, installIcon);
        }
        for (const QString &package: item.upgradedPackages()) {
            addChildRow(package, QApt::Package::ToUpgrade, upgradeIcon);
        }
        for (const QString &package: item.downgradedPackages()) {
            addChildRow(package, QApt::Package::ToDowngrade, downgradeIcon);
        }
        for (const QString &package: item.removedPackages()) {
            addChildRow(package, QApt::Package::ToRemove, removeIcon);
        }
        for (const QString &package: item.purgedPackages()) {
            addChildRow(package, QApt::Package::ToPurge, purgeIcon);
        }
        for (const QString &package: item.reinstalledPackages()) {
            addChildRow(package, QApt::Package::ToReInstall, reinstallIcon);
        }
    }

    viewLayout->addWidget(m_historyView);

    m_proxyModel = new HistoryProxyModel(this);
    m_proxyModel->setSourceModel(m_historyModel);
    m_proxyModel->sort(0);

    m_historyView->setModel(m_proxyModel);

    connect(m_proxyModel, &QAbstractItemModel::layoutChanged, this, &HistoryView::updateSpanning);
    connect(m_proxyModel, &QAbstractItemModel::modelReset, this, &HistoryView::updateSpanning);

    updateSpanning();

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

QSize HistoryView::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(750, 500));
}

void HistoryView::setStateFilter(int index)
{
    QApt::Package::State state = (QApt::Package::State)m_filterBox->itemData(index).toInt();
    m_proxyModel->setStateFilter(state);
}

void HistoryView::startSearch()
{
    m_proxyModel->search(m_searchEdit->text());
}

void HistoryView::updateItemColors(QStandardItem *item, const KColorScheme &scheme)
{
    if (item->data(HistoryProxyModel::HistoryActionRole).isValid()) {
        int action = item->data(HistoryProxyModel::HistoryActionRole).toInt();
        QColor color;
        switch (action) {
            case QApt::Package::ToInstall:
                color = scheme.foreground(KColorScheme::PositiveText).color();
                break;
            case QApt::Package::ToUpgrade:
                color = scheme.decoration(KColorScheme::FocusColor).color();
                break;
            case QApt::Package::ToDowngrade:
                color = scheme.foreground(KColorScheme::NeutralText).color();
                break;
            case QApt::Package::ToRemove:
            case QApt::Package::ToPurge:
                color = scheme.foreground(KColorScheme::NegativeText).color();
                break;
            case QApt::Package::ToReInstall:
                color = scheme.foreground(KColorScheme::VisitedText).color();
                break;
            default:
                color = scheme.foreground(KColorScheme::NormalText).color();
                break;
        }
        item->setData(color, Qt::ForegroundRole);
    }

    for (int row = 0; row < item->rowCount(); ++row) {
        int cols = item->columnCount();
        for (int col = 0; col < cols; ++col) {
            QStandardItem *child = item->child(row, col);
            if (child)
                updateItemColors(child, scheme);
        }
    }
}

bool HistoryView::event(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateAllItemColors();
    }
    return QWidget::event(event);
}

void HistoryView::updateAllItemColors()
{
    m_historyView->setStyleSheet(itemStyleSheet);

    m_colorScheme = KColorScheme(QPalette::Current, KColorScheme::Window);
    for (int i = 0; i < m_historyModel->rowCount(); ++i) {
        QStandardItem *item = m_historyModel->item(i);
        updateItemColors(item, m_colorScheme);
    }
}

void HistoryView::updateSpanning()
{
    for (int row = 0; row < m_historyModel->rowCount(); ++row) {
        QStandardItem *parentItem = m_historyModel->item(row);

        if (parentItem && parentItem->hasChildren()) {
            m_historyView->setFirstColumnSpanned(row, QModelIndex(), true);
        }
    }

    m_historyView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int col = 1; col < m_historyModel->columnCount(); ++col) {
        m_historyView->header()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }
}


