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

#include <KColorScheme>
#include <KLocalizedString>

#include <QApt/History>

#include "HistoryProxyModel.h"

HistoryView::HistoryView(QWidget *parent)
    : QWidget(parent),
      m_colorScheme(QPalette::Current, KColorScheme::Window),
      m_configWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
{
    connect(m_configWatcher.data(), &KConfigWatcher::configChanged, this, &HistoryView::updateAllItemColors);

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
    connect(m_searchTimer, SIGNAL(timeout()), this, SLOT(startSearch()));
    connect(m_searchEdit, SIGNAL(textChanged(QString)), m_searchTimer, SLOT(start()));

    m_filterBox = new QComboBox(headerWidget);
    m_filterBox->insertItem(AllChangesItem, QIcon::fromTheme(QStringLiteral("bookmark-new-list")),
                            i18nc("@item:inlistbox Filters all changes in the history view",
                                  "All changes"),
                            0);
    m_filterBox->insertItem(InstallationsItem, QIcon::fromTheme(QStringLiteral("download")),
                            i18nc("@item:inlistbox Filters installations in the history view",
                                  "Installations"),
                            QApt::Package::ToInstall);
    m_filterBox->insertItem(UpdatesItem, QIcon::fromTheme(QStringLiteral("system-software-update")),
                            i18nc("@item:inlistbox Filters updates in the history view",
                                  "Updates"),
                            QApt::Package::ToUpgrade);
    m_filterBox->insertItem(RemovalsItem, QIcon::fromTheme(QStringLiteral("edit-delete")),
                            i18nc("@item:inlistbox Filters removals in the history view",
                                  "Removals"),
                            (QApt::Package::State)(QApt::Package::ToRemove | QApt::Package::ToPurge));
    connect(m_filterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setStateFilter(int)));

    headerLayout->addWidget(headerLabel);
    headerLayout->addWidget(headerSpacer);
    headerLayout->addWidget(m_searchEdit);
    headerLayout->addWidget(m_filterBox);

    m_historyModel = new QStandardItemModel(this);
    m_historyModel->setColumnCount(1);
    m_historyModel->setHeaderData(0, Qt::Horizontal, i18nc("@title:column", "Date"));
    m_historyView = new QTreeView(this);

    QIcon itemIcon(QIcon::fromTheme(QStringLiteral("applications-other")));

    QHash<QString, QString> categoryHash;

    QHash<PastActions, QString> actionHash;
    actionHash[InstalledAction] = i18nc("@info:status describes a past-tense action", "Installed");
    actionHash[UpgradedAction] = i18nc("@info:status describes a past-tense action", "Upgraded");
    actionHash[DowngradedAction] = i18nc("@status describes a past-tense action", "Downgraded");
    actionHash[RemovedAction] = i18nc("@status describes a past-tense action", "Removed");
    actionHash[PurgedAction] = i18nc("@status describes a past-tense action", "Purged");

    for (const QApt::HistoryItem &item: m_history->historyItems()) {
        QDateTime startDateTime = item.startDate();
        QString formattedTime = startDateTime.toString();
        QString category;

        QString date = startDateTime.date().toString();
        if (categoryHash.contains(date)) {
            category = categoryHash.value(date);
        } else {
            category = QLocale().toString(startDateTime.date(), QLocale::ShortFormat);
            categoryHash[date] = category;
        }

        QStandardItem *parentItem = nullptr;

        if (!m_categoryHash.contains(category)) {
            parentItem = new QStandardItem;
            parentItem->setEditable(false);
            parentItem->setText(category);
            parentItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);

            m_historyModel->appendRow(parentItem);
            m_categoryHash[category] = parentItem;
        } else {
            parentItem = m_categoryHash.value(category);
        }

        for (const QString &package: item.installedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(itemIcon);

            QString action = actionHash.value(InstalledAction);
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToInstall, HistoryProxyModel::HistoryActionRole);

            updateItemColors(historyItem, m_colorScheme);

            parentItem->appendRow(historyItem);
        }

        for (const QString &package: item.upgradedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(itemIcon);

            QString action = actionHash.value(UpgradedAction);
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToUpgrade, HistoryProxyModel::HistoryActionRole);

            updateItemColors(historyItem, m_colorScheme);

            parentItem->appendRow(historyItem);
        }

        for (const QString &package: item.downgradedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(itemIcon);

            QString action = actionHash.value(DowngradedAction);
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToDowngrade, HistoryProxyModel::HistoryActionRole);

            updateItemColors(historyItem, m_colorScheme);

            parentItem->appendRow(historyItem);
        }

        for (const QString &package: item.removedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(itemIcon);

            QString action = actionHash.value(RemovedAction);
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToRemove, HistoryProxyModel::HistoryActionRole);

            updateItemColors(historyItem, m_colorScheme);

            parentItem->appendRow(historyItem);
        }

        for (const QString &package: item.purgedPackages()) {
            QStandardItem *historyItem = new QStandardItem;
            historyItem->setEditable(false);
            historyItem->setIcon(itemIcon);

            QString action = actionHash.value(PurgedAction);
            QString text = i18nc("@item example: muon installed at 16:00", "%1 %2 at %3",
                                 package, action, formattedTime);
            historyItem->setText(text);
            historyItem->setData(startDateTime, HistoryProxyModel::HistoryDateRole);
            historyItem->setData(QApt::Package::ToPurge, HistoryProxyModel::HistoryActionRole);

            updateItemColors(historyItem, m_colorScheme);

            parentItem->appendRow(historyItem);
        }
    }

    m_historyView->setMouseTracking(true);
    m_historyView->setVerticalScrollMode(QListView::ScrollPerPixel);

    viewLayout->addWidget(headerWidget);
    viewLayout->addWidget(m_historyView);

    m_proxyModel = new HistoryProxyModel(this);
    m_proxyModel->setSourceModel(m_historyModel);
    m_proxyModel->sort(0);

    m_historyView->setModel(m_proxyModel);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

QSize HistoryView::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(500, 500));
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
        switch(action) {
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
            default:
                color = scheme.foreground(KColorScheme::NormalText).color();
                break;
        }
        item->setData(color, Qt::ForegroundRole);
    }

    for (int i = 0; i < item->rowCount(); ++i) {
        updateItemColors(item->child(i), scheme);
    }
}

void HistoryView::updateAllItemColors()
{
    m_colorScheme = KColorScheme(QPalette::Current, KColorScheme::Window);
    for (int i = 0; i < m_historyModel->rowCount(); ++i) {
        QStandardItem *item = m_historyModel->item(i);
        updateItemColors(item, m_colorScheme);
    }
}


