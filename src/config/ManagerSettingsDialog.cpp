/***************************************************************************
 *   Copyright (C) 2006 by Peter Penz <peter.penz@gmx.at>                  *
 *   Copyright (C) 2010 by Jonathan Thomas <echidnaman@kubuntu.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "ManagerSettingsDialog.h"

#include <QPushButton>
#include <KLocalizedString>

#include <QApt/Config>

#include "../settings/SettingsPageBase.h"
#include "GeneralSettingsPage.h"

ManagerSettingsDialog::ManagerSettingsDialog(QWidget* parent, QApt::Config *aptConfig) :
    KPageDialog(parent),
    m_aptConfig(aptConfig)

{
    const QSize minSize = minimumSize();
    setMinimumSize(QSize(512, minSize.height()));

    setFaceType(Tabbed);
    setWindowTitle(i18nc("@title:window", "Muon Preferences"));
    setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
    button(QDialogButtonBox::Apply)->setEnabled(false);

    // General settings
    GeneralSettingsPage *generalPage = new GeneralSettingsPage(this, m_aptConfig);
    KPageWidgetItem *generalSettingsFrame = addPage(generalPage,
                                                    i18nc("@title:group Title of the general group", "General"));
    generalSettingsFrame->setIcon(QIcon::fromTheme(QStringLiteral("system-run")));
    connect(generalPage, SIGNAL(changed()), this, SLOT(changed()));
    connect(generalPage, SIGNAL(authChanged()), this, SLOT(authChanged()));
    connect(buttonBox(), SIGNAL(clicked(QAbstractButton*)), this, SLOT(slotButtonClicked(QAbstractButton*)));

    m_pages.insert(generalPage);
}

ManagerSettingsDialog::~ManagerSettingsDialog()
{
}

void ManagerSettingsDialog::slotButtonClicked(QAbstractButton* b)
{
    if ((b == button(QDialogButtonBox::Ok)) || (b == button(QDialogButtonBox::Apply))) {
        applySettings();
    } else if (b == button(QDialogButtonBox::RestoreDefaults)) {
        restoreDefaults();
    }
}

void ManagerSettingsDialog::changed()
{
    button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    button(QDialogButtonBox::Apply)->setEnabled(true);
}

void ManagerSettingsDialog::authChanged()
{
    button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-password")));
    button(QDialogButtonBox::Apply)->setEnabled(true);
}

void ManagerSettingsDialog::applySettings()
{
    for (SettingsPageBase* page: m_pages) {
        page->applySettings();
    }

    Q_EMIT settingsChanged();
    button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    button(QDialogButtonBox::Apply)->setEnabled(false);
}

void ManagerSettingsDialog::restoreDefaults()
{
    for (SettingsPageBase* page: m_pages) {
        page->restoreDefaults();
    }

    button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
}

#include "ManagerSettingsDialog.moc"
