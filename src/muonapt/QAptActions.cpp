/***************************************************************************
 *   Copyright © 2012 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "QAptActions.h"
#include "MuonStrings.h"
#include "HistoryView/HistoryView.h"

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QStringBuilder>
#include <QAction>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QLayout>
#include <QNetworkInformation>
#include <QTimer>

// KDE includes
#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
#include <KSharedConfig>
#include <KXmlGuiWindow>
#include <KWindowConfig>

// QApt includes
#include <QApt/Backend>
#include <QApt/DebFile>
#include <QApt/Transaction>

#include "muonapt/HistoryView/HistoryDialog.h"

QAptActions::QAptActions()
    : QObject(nullptr)
    , m_backend(nullptr)
    , m_actionsDisabled(false)
    , m_mainWindow(nullptr)
    , m_reloadWhenEditorFinished(false)
    , m_historyDialog(nullptr)
    , m_distUpgradeAvailable(false)
{
    QNetworkInformation::loadDefaultBackend();
    connect(QNetworkInformation::instance(), &QNetworkInformation::reachabilityChanged, this, &QAptActions::reachabilityChanged);
}

QAptActions* QAptActions::self()
{
    static QPointer<QAptActions> self;
    if(!self) {
        self = new QAptActions;
    }
    return self;
}

void QAptActions::setMainWindow(KXmlGuiWindow* w)
{
    setParent(w);
    m_mainWindow = w;

    setupActions();
}

KXmlGuiWindow* QAptActions::mainWindow() const
{
    return m_mainWindow;
}

void QAptActions::setBackend(QApt::Backend* backend)
{
    if(backend == m_backend)
        return;
    m_backend = backend;
    if (!m_backend->init())
        initError();

    connect(m_backend, SIGNAL(packageChanged()), this, SLOT(setActionsEnabled()));

    setOriginalState(m_backend->currentCacheState());

    setReloadWhenEditorFinished(true);
    // Some actions need an initialized backend to be able to set their enabled state
    setActionsEnabled(true);
    checkDistUpgrade();
}

void QAptActions::setupActions()
{
    QAction* undoAction = KStandardAction::undo(this, SLOT(undo()), actionCollection());
    actionCollection()->addAction(QStringLiteral("undo"), undoAction);
    m_actions.append(undoAction);

    QAction* redoAction = KStandardAction::redo(this, SLOT(redo()), actionCollection());
    actionCollection()->addAction(QStringLiteral("redo"), redoAction);
    m_actions.append(redoAction);

    QAction* revertAction = actionCollection()->addAction(QStringLiteral("revert"));
    revertAction->setIcon(QIcon::fromTheme(QStringLiteral("document-revert")));
    revertAction->setText(i18nc("@action Reverts all potential changes to the cache", "Unmark All"));
    connect(revertAction, SIGNAL(triggered()), this, SLOT(revertChanges()));
    m_actions.append(revertAction);

    QAction* softwarePropertiesAction = actionCollection()->addAction(QStringLiteral("software_properties"));
    softwarePropertiesAction->setPriority(QAction::LowPriority);
    softwarePropertiesAction->setIcon(QIcon::fromTheme(QStringLiteral("configure")));
    softwarePropertiesAction->setText(i18nc("@action Opens the software sources configuration dialog", "Configure Software Sources"));
    connect(softwarePropertiesAction, SIGNAL(triggered()), this, SLOT(runSourcesEditor()));
    m_actions.append(softwarePropertiesAction);

    QAction* loadSelectionsAction = actionCollection()->addAction(QStringLiteral("open_markings"));
    loadSelectionsAction->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    loadSelectionsAction->setText(i18nc("@action", "Read Markings..."));
    connect(loadSelectionsAction, SIGNAL(triggered()), this, SLOT(loadSelections()));
    m_actions.append(loadSelectionsAction);

    QAction* saveSelectionsAction = actionCollection()->addAction(QStringLiteral("save_markings"));
    saveSelectionsAction->setIcon(QIcon::fromTheme(QStringLiteral("document-save-as")));
    saveSelectionsAction->setText(i18nc("@action", "Save Markings As..."));
    connect(saveSelectionsAction, SIGNAL(triggered()), this, SLOT(saveSelections()));
    m_actions.append(saveSelectionsAction);

    QAction* createDownloadListAction = actionCollection()->addAction(QStringLiteral("save_download_list"));
    createDownloadListAction->setPriority(QAction::LowPriority);
    createDownloadListAction->setIcon(QIcon::fromTheme(QStringLiteral("document-save-as")));
    createDownloadListAction->setText(i18nc("@action", "Save Package Download List..."));
    connect(createDownloadListAction, SIGNAL(triggered()), this, SLOT(createDownloadList()));
    m_actions.append(createDownloadListAction);

    QAction* downloadListAction = actionCollection()->addAction(QStringLiteral("download_from_list"));
    downloadListAction->setPriority(QAction::LowPriority);
    downloadListAction->setIcon(QIcon::fromTheme(QStringLiteral("download")));
    downloadListAction->setText(i18nc("@action", "Download Packages From List..."));
    connect(downloadListAction, SIGNAL(triggered()), this, SLOT(downloadPackagesFromList()));
    downloadListAction->setEnabled(isConnected());
    connect(this, SIGNAL(shouldConnect(bool)), downloadListAction, SLOT(setEnabled(bool)));
    m_actions.append(downloadListAction);

    QAction* loadArchivesAction = actionCollection()->addAction(QStringLiteral("load_archives"));
    loadArchivesAction->setPriority(QAction::LowPriority);
    loadArchivesAction->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    loadArchivesAction->setText(i18nc("@action", "Add Downloaded Packages"));
    connect(loadArchivesAction, SIGNAL(triggered()), this, SLOT(loadArchives()));
    m_actions.append(loadArchivesAction);

    QAction* saveInstalledAction = actionCollection()->addAction(QStringLiteral("save_package_list"));
    saveInstalledAction->setPriority(QAction::LowPriority);
    saveInstalledAction->setIcon(QIcon::fromTheme(QStringLiteral("document-save-as")));
    saveInstalledAction->setText(i18nc("@action", "Save Installed Packages List..."));
    connect(saveInstalledAction, SIGNAL(triggered()), this, SLOT(saveInstalledPackagesList()));

    QAction* historyAction = actionCollection()->addAction(QStringLiteral("history"));
    historyAction->setPriority(QAction::LowPriority);
    historyAction->setIcon(QIcon::fromTheme(QStringLiteral("view-history")));
    historyAction->setText(i18nc("@action::inmenu", "History..."));
    KActionCollection::setDefaultShortcut(historyAction, QKeySequence(Qt::CTRL | Qt::Key_H));
    connect(historyAction, SIGNAL(triggered()), this, SLOT(showHistoryDialog()));

    QAction *distUpgradeAction = actionCollection()->addAction(QStringLiteral("dist-upgrade"));
    distUpgradeAction->setIcon(QIcon::fromTheme(QStringLiteral("system-software-update")));
    distUpgradeAction->setText(i18nc("@action", "Upgrade"));
    distUpgradeAction->setPriority(QAction::HighPriority);
    distUpgradeAction->setWhatsThis(i18nc("Notification when a new version of Kubuntu is available",
                                        "A new version of Kubuntu is available."));
    distUpgradeAction->setEnabled(m_distUpgradeAvailable);
    connect(distUpgradeAction, SIGNAL(triggered(bool)), SLOT(launchDistUpgrade()));

    m_actions.append(saveInstalledAction);
}

void QAptActions::setActionsEnabled(bool enabled)
{
    m_actionsDisabled = !enabled;

    for (QAction *action: m_actions) {
        action->setEnabled(enabled);
    }

    if (!enabled || !m_mainWindow || !actionCollection())
        return;

    actionCollection()->action(QStringLiteral("update"))->setEnabled(isConnected() && enabled);

    actionCollection()->action(QStringLiteral("undo"))->setEnabled(m_backend && !m_backend->isUndoStackEmpty());
    actionCollection()->action(QStringLiteral("redo"))->setEnabled(m_backend && !m_backend->isRedoStackEmpty());
    actionCollection()->action(QStringLiteral("revert"))->setEnabled(m_backend && m_backend->areChangesMarked());

    actionCollection()->action(QStringLiteral("save_download_list"))->setEnabled(isConnected());

    bool changesPending = m_backend && m_backend->areChangesMarked();
    actionCollection()->action(QStringLiteral("save_markings"))->setEnabled(changesPending);
    actionCollection()->action(QStringLiteral("save_download_list"))->setEnabled(changesPending);
    actionCollection()->action(QStringLiteral("dist-upgrade"))->setEnabled(m_distUpgradeAvailable);
}

bool QAptActions::reloadWhenSourcesEditorFinished() const
{
    return m_reloadWhenEditorFinished;
}

bool QAptActions::isConnected() const
{
    return QNetworkInformation::instance()->reachability() == QNetworkInformation::Reachability::Online;
}

bool QAptActions::saveSelections()
{
    QString filename = QFileDialog::getSaveFileName(m_mainWindow, i18nc("@title:window", "Save Markings As"));

    if (filename.isEmpty()) {
        return false;
    }

    if (!m_backend->saveSelections(filename)) {
        QString text = xi18nc("@label", "The document could not be saved, as it "
                             "was not possible to write to "
                             "<filename>%1</filename>\n\nCheck "
                             "that you have write access to this file "
                             "or that enough disk space is available.",
                             filename);
        KMessageBox::error(m_mainWindow, text, QString());
        return false;
    }

    return true;
}

bool QAptActions::saveInstalledPackagesList()
{
    QString filename;

    filename = QFileDialog::getSaveFileName(m_mainWindow,
                                            i18nc("@title:window", "Save Installed Packages List As"));

    if (filename.isEmpty()) {
        return false;
    }

    if (!m_backend->saveInstalledPackagesList(filename)) {
        QString text = xi18nc("@label", "The document could not be saved, as it "
                             "was not possible to write to "
                             "<filename>%1</filename>\n\nCheck "
                             "that you have write access to this file "
                             "or that enough disk space is available.",
                             filename);
        KMessageBox::error(m_mainWindow, text, QString());
        return false;
    }

    return true;
}

bool QAptActions::createDownloadList()
{
    QString filename;
    filename = QFileDialog::getSaveFileName(m_mainWindow,
                                            i18nc("@title:window", "Save Download List As"));

    if (filename.isEmpty()) {
        return false;
    }

    if (!m_backend->saveDownloadList(filename)) {
        QString text = xi18nc("@label", "The document could not be saved, as it "
                             "was not possible to write to "
                             "<filename>%1</filename>\n\nCheck "
                             "that you have write access to this file "
                             "or that enough disk space is available.",
                             filename);
        KMessageBox::error(m_mainWindow, text, QString());
        return false;
    }

    return true;
}

void QAptActions::downloadPackagesFromList()
{
    QString filename = QFileDialog::getOpenFileName(m_mainWindow, i18nc("@title:window", "Open File"));

    if (filename.isEmpty()) {
        return;
    }

    QString dirName = filename.left(filename.lastIndexOf(QChar::fromLatin1('/')));

    setActionsEnabled(false);
    QApt::Transaction *trans = m_backend->downloadArchives(filename, dirName % QLatin1String("/packages"));

    if (trans)
        Q_EMIT downloadArchives(trans);
}

void QAptActions::loadSelections()
{
    QString filename = QFileDialog::getOpenFileName(m_mainWindow, i18nc("@title:window", "Open File"));

    if (filename.isEmpty()) {
        return;
    }

    m_backend->saveCacheState();
    if (!m_backend->loadSelections(filename)) {
        QString text = i18nc("@label", "Could not mark changes. Please make sure "
                             "that the file is a markings file created by "
                             "either the Muon Package Manager or the "
                             "Synaptic Package Manager.");
        KMessageBox::error(m_mainWindow, text, QString());
    }
}

void QAptActions::loadArchives()
{
    QString dirName = QFileDialog::getExistingDirectory(m_mainWindow,
                                                i18nc("@title:window", "Choose a Directory"));

    if (dirName.isEmpty()) {
        // User canceled
        return;
    }

    QDir dir(dirName);
    QStringList archiveFiles = dir.entryList(QDir::Files, QDir::Name);

    int successCount = 0;
    for (const QString &archiveFile: archiveFiles) {
        const QApt::DebFile debFile(dirName % QChar::fromLatin1('/') % archiveFile);

        if (debFile.isValid()) {
            if (m_backend->addArchiveToCache(debFile)) {
                successCount++;
            }
        }
    }

    if (successCount) {
        QString message = i18ncp("@label",
                                 "%1 package was successfully added to the cache",
                                 "%1 packages were successfully added to the cache",
                                 successCount);
        KMessageBox::information(m_mainWindow, message, QString());
    } else {
        QString message = i18nc("@label",
                                "No valid packages could be found in this directory. "
                                "Please make sure the packages are compatible with your "
                                "computer and are at the latest version.");
        KMessageBox::error(m_mainWindow, message, i18nc("@title:window",
                                                "Packages Could Not be Found"));
    }
}

void QAptActions::undo()
{
    m_backend->undo();
}

void QAptActions::redo()
{
    m_backend->redo();
}

void QAptActions::revertChanges()
{
    m_backend->saveCacheState();
    m_backend->restoreCacheState(m_originalState);
    Q_EMIT changesReverted();
}

void QAptActions::runSourcesEditor()
{
    WId winID = m_mainWindow->effectiveWinId();

    QString editor = QStandardPaths::findExecutable(QStringLiteral("software-properties-qt"));
    if (editor.isEmpty()) {
        editor = QStandardPaths::findExecutable(QStringLiteral("software-properties-kde"));
        if (editor.isEmpty()) {
            QString text = xi18nc("@info",
                                  "<para>Could not find <command>software-properties-qt</command> "
                                  "nor <command>software-properties-kde</command> "
                                  "on your system, please install it.</para>");
            QString title = xi18nc("@title:window",
                                   "Cannot find <command>software-properties-qt</command>");
            KMessageBox::information(m_mainWindow, text, title);
            return;
        }
    }

    QString pkexec = QStandardPaths::findExecutable(QStringLiteral("pkexec"));
    if (pkexec.isEmpty()) {
        KMessageBox::error(m_mainWindow,
                           i18n("pkexec could not be found. Please install polkit-1."),
                           i18n("pkexec not found"));
        return;
    }

    QStringList args;
    args << QStringLiteral("env");
    args << QStringLiteral("DISPLAY=%1").arg(QString::fromUtf8(qgetenv("DISPLAY")));
    args << QStringLiteral("XAUTHORITY=%1").arg(QString::fromUtf8(qgetenv("XAUTHORITY")));
    args << QStringLiteral("WAYLAND_DISPLAY=%1").arg(QString::fromUtf8(qgetenv("WAYLAND_DISPLAY")));
    args << QStringLiteral("XDG_CURRENT_DESKTOP=%1").arg(QString::fromUtf8(qgetenv("XDG_CURRENT_DESKTOP")));
    args << QStringLiteral("XDG_CONFIG_DIRS=%1").arg(QString::fromUtf8(qgetenv("XDG_CONFIG_DIRS")));

    args << editor;
    args << QStringLiteral("--attach") << QString::number(winID);
    if (m_reloadWhenEditorFinished)
        args << QStringLiteral("--dont-update");

    qDebug() << "Executing pkexec command:" << pkexec << args;

    QProcess *proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &QAptActions::sourcesEditorFinished);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            proc, &QProcess::deleteLater);

    if (auto *win = KXmlGuiWindow::find(winID)) {
        win->setEnabled(false);
    } else {
        qWarning() << "No window found for winID:" << winID;
    }

    proc->setProcessChannelMode(QProcess::ForwardedChannels);

    proc->start(pkexec, args);
}

void QAptActions::sourcesEditorFinished(int exitStatus)
{
    bool reload = (exitStatus == 1);
    KXmlGuiWindow::find(m_mainWindow->effectiveWinId())->setEnabled(true);
    if (m_reloadWhenEditorFinished && reload) {
        actionCollection()->action(QStringLiteral("update"))->trigger();
    }

    Q_EMIT sourcesEditorClosed(reload);
}

KActionCollection* QAptActions::actionCollection()
{
    return m_mainWindow->actionCollection();
}

void QAptActions::setOriginalState(QApt::CacheState state)
{
    m_originalState = state;
}

void QAptActions::setReloadWhenEditorFinished(bool reload)
{
    m_reloadWhenEditorFinished = reload;
}

void QAptActions::initError()
{
    QString details = m_backend->initErrorMessage();

    MuonStrings *muonStrings = MuonStrings::global();

    QString title = muonStrings->errorTitle(QApt::InitError);
    QString text = muonStrings->errorText(QApt::InitError, nullptr);

    KMessageBox::detailedError(m_mainWindow, text, details, title);
    exit(-1);
}

void QAptActions::displayTransactionError(QApt::ErrorCode error, QApt::Transaction* trans)
{
    if (error == QApt::Success)
        return;

    MuonStrings *muonStrings = MuonStrings::global();

    QString title = muonStrings->errorTitle(error);
    QString text = muonStrings->errorText(error, trans);

    switch (error) {
        case QApt::InitError:
        case QApt::FetchError:
        case QApt::CommitError:
            KMessageBox::detailedError(QAptActions::self()->mainWindow(), text, trans->errorDetails(), title);
            break;
        default:
            KMessageBox::error(QAptActions::self()->mainWindow(), text, title);
            break;
    }
}

void QAptActions::showHistoryDialog()
{
    if (!m_historyDialog) {
        m_historyDialog = new HistoryDialog(mainWindow());
        connect(m_historyDialog, &QDialog::finished, this, [this](int){
            m_historyDialog->deleteLater();
            m_historyDialog = nullptr;
        });
        m_historyDialog->show();
    } else {
        m_historyDialog->raise();
    }
}

// TODO: replace with /usr/bin/kubuntu-devel-release-upgrade and ubuntu-release-upgrader
void QAptActions::launchDistUpgrade()
{
    const QString kdesu = QFile::decodeName(CMAKE_INSTALL_FULL_LIBEXECDIR_KF6 "/kdesu");
    QProcess::startDetached(kdesu, {QStringLiteral("--"), QStringLiteral("do-release-upgrade"), QStringLiteral("-m"), QStringLiteral("desktop"), QStringLiteral("-f"), QStringLiteral("DistUpgradeViewKDE")});
}

void QAptActions::checkDistUpgrade()
{
    if(!QFile::exists(QStringLiteral("/usr/lib/python3/dist-packages/DistUpgrade/DistUpgradeFetcherKDE.py"))) {
        qWarning() << "Couldn't find the /usr/lib/python3/dist-packages/DistUpgrade/DistUpgradeFetcherKDE.py file";
        return;
    }
    QString checkerFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("libmuon/applicationsbackend/releasechecker"));
    if(checkerFile.isEmpty()) {
        qWarning() << "Couldn't find the releasechecker script" << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
        return;
    }

    QProcess *checkerProcess = new QProcess(this);
    checkerProcess->setProgram(QStringLiteral("/usr/bin/python3"));
    checkerProcess->setArguments(QStringList() << checkerFile);
    checkerProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(checkerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &QAptActions::checkerFinished);
    connect(checkerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            checkerProcess, &QProcess::deleteLater);
    checkerProcess->start();
}

void QAptActions::checkerFinished(int res)
{
    m_distUpgradeAvailable = (res == 0);
    if (!m_mainWindow)
        return;
    actionCollection()->action(QStringLiteral("dist-upgrade"))->setEnabled(m_distUpgradeAvailable);
}

void QAptActions::reachabilityChanged(QNetworkInformation::Reachability reachability)
{
    Q_EMIT shouldConnect(reachability == QNetworkInformation::Reachability::Online);
}
