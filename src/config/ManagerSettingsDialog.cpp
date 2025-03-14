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

#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QAbstractButton>
#include <QIcon>
#include <KLocalizedString>

#include <QApt/Config>

#include "../settings/SettingsPageBase.h"
#include "GeneralSettingsPage.h"

ManagerSettingsDialog::ManagerSettingsDialog(QWidget* parent, QApt::Config *aptConfig) :
    QDialog(parent),
    m_aptConfig(aptConfig)

{
    setWindowTitle(i18nc("@title:window", "Muon Preferences"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply
                                       | QDialogButtonBox::Cancel
                                       | QDialogButtonBox::RestoreDefaults,
                                       this);
    mainLayout->addWidget(m_buttonBox);

    if (QPushButton *applyButton = m_buttonBox->button(QDialogButtonBox::Apply))
        applyButton->setEnabled(false);

    connect(m_buttonBox, &QDialogButtonBox::clicked, this, &ManagerSettingsDialog::slotButtonClicked);

    GeneralSettingsPage *generalPage = new GeneralSettingsPage(this, m_aptConfig);
    m_tabWidget->addTab(generalPage, QIcon::fromTheme(QStringLiteral("system-run")), i18nc("@title:group Title of the general group", "General"));
    m_pages.insert(generalPage);

    connect(generalPage, SIGNAL(changed()), this, SLOT(changed()));
    connect(generalPage, SIGNAL(authChanged()), this, SLOT(authChanged()));
}

ManagerSettingsDialog::~ManagerSettingsDialog()
{
}

void ManagerSettingsDialog::slotButtonClicked(QAbstractButton* b)
{
    if (b == m_buttonBox->button(QDialogButtonBox::Apply) ||
        b == m_buttonBox->button(QDialogButtonBox::Ok)) {
        applySettings();
    } else if (b == m_buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        restoreDefaults();
    } else if (b == m_buttonBox->button(QDialogButtonBox::Cancel)) {
        reject();
    }
}

void ManagerSettingsDialog::changed()
{
    if (QPushButton *applyButton = m_buttonBox->button(QDialogButtonBox::Apply)) {
        applyButton->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
        applyButton->setEnabled(true);
    }
}

void ManagerSettingsDialog::authChanged()
{
    if (QPushButton *applyButton = m_buttonBox->button(QDialogButtonBox::Apply)) {
        applyButton->setIcon(QIcon::fromTheme(QStringLiteral("dialog-password")));
        applyButton->setEnabled(true);
    }
}

void ManagerSettingsDialog::applySettings()
{
    for (SettingsPageBase* page: m_pages) {
        page->applySettings();
    }

    Q_EMIT settingsChanged();
    if (QPushButton *applyButton = m_buttonBox->button(QDialogButtonBox::Apply)) {
        applyButton->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
        applyButton->setEnabled(false);
    }
}

void ManagerSettingsDialog::restoreDefaults()
{
    for (SettingsPageBase* page: m_pages) {
        page->restoreDefaults();
    }

    if (QPushButton *applyButton = m_buttonBox->button(QDialogButtonBox::Apply)) {
        applyButton->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    }
}

#include "moc_ManagerSettingsDialog.cpp"
