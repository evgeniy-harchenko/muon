#include <QDialog>
#include <QVBoxLayout>
#include <KWindowConfig>
#include <KSharedConfig>
#include <KLocalizedString>
#include "HistoryView.h"
#include "HistoryDialog.h"
#include <QTimer>
#include <QWindow>

HistoryDialog::HistoryDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(i18nc("@title:window", "Package History"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("view-history")));
    setLayout(new QVBoxLayout(this));

    HistoryView *view = new HistoryView(this);
    this->layout()->addWidget(view);

    create(); // ensure there's a window created

    KConfigGroup dialogConfig(KSharedConfig::openConfig(QStringLiteral("muonrc")), QStringLiteral("HistoryDialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), dialogConfig);
    KWindowConfig::restoreWindowPosition(windowHandle(), dialogConfig);

    // workaround for QTBUG-40584
    if (windowHandle()) {
        setGeometry(windowHandle()->geometry());
    }
}

HistoryDialog::~HistoryDialog() {
    if (windowHandle()) {
        KConfigGroup dialogConfig(KSharedConfig::openConfig(QStringLiteral("muonrc")), QStringLiteral("HistoryDialog"));
        KWindowConfig::saveWindowSize(windowHandle(), dialogConfig);
        KWindowConfig::saveWindowPosition(windowHandle(), dialogConfig);
    }
}