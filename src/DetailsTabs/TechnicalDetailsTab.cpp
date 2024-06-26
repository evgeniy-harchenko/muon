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

#include "TechnicalDetailsTab.h"

// Qt includes
#include <QGridLayout>
#include <QGroupBox>
#include <QtWidgets/QLabel>
#include <QScrollArea>

// KDE includes
#include <KFormat>
#include <KLocalizedString>

// QApt includes
#include <QApt/Backend>
#include <QApt/Package>

// Own includes
#include "muonapt/MuonStrings.h"

TechnicalDetailsTab::TechnicalDetailsTab(QWidget *parent)
    : DetailsTab(parent)
{
    m_name = i18nc("@title:tab", "Technical Details");

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->viewport()->setAutoFillBackground(false);
    m_layout->addWidget(scrollArea);

    QWidget *generalWidget = new QWidget;
    QGridLayout *generalGrid = new QGridLayout(generalWidget);
    generalWidget->setLayout(generalGrid);

    // generalGrid, row 0
    QLabel *maintainerLabel = new QLabel(generalWidget);
    maintainerLabel->setText(i18nc("@label Label preceding the package maintainer", "Maintainer:"));
    m_maintainer = new QLabel(generalWidget);
    generalGrid->addWidget(maintainerLabel, 0, 0, Qt::AlignRight);
    generalGrid->addWidget(m_maintainer, 0, 1, Qt::AlignLeft);
    maintainerLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_maintainer->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // generalGrid, row 1
    QLabel *sectionLabel = new QLabel(generalWidget);
    sectionLabel->setText(i18nc("@label Label preceding the package category", "Category:"));
    m_section = new QLabel(generalWidget);
    generalGrid->addWidget(sectionLabel, 1, 0, Qt::AlignRight);
    generalGrid->addWidget(m_section, 1, 1, Qt::AlignLeft);
    sectionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_section->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // generalGrid, row 2
    QLabel *sourcePackageLabel = new QLabel(generalWidget);
    sourcePackageLabel->setText(i18nc("@label The parent package that this package comes from",
                                      "Source Package:"));
    m_sourcePackage = new QLabel(generalWidget);
    generalGrid->addWidget(sourcePackageLabel, 2, 0, Qt::AlignRight);
    generalGrid->addWidget(m_sourcePackage, 2, 1, Qt::AlignLeft);
    sourcePackageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_sourcePackage->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // generalGrid, row 3
    QLabel *originLabel = new QLabel(generalWidget);
    originLabel->setText(i18nc("@label The software source that this package comes from",
                                      "Origin:"));
    m_origin = new QLabel(generalWidget);
    generalGrid->addWidget(originLabel, 3, 0, Qt::AlignRight);
    generalGrid->addWidget(m_origin, 3, 1, Qt::AlignLeft);
    originLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_origin->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // generalGrid, row 4
    QLabel *componentLabel = new QLabel(generalWidget);
    componentLabel->setText(i18nc("@label The package archive component that this package comes from",
                                      "Component:"));
    m_component = new QLabel(generalWidget);
    generalGrid->addWidget(componentLabel, 4, 0, Qt::AlignRight);
    generalGrid->addWidget(m_component, 4, 1, Qt::AlignLeft);
    componentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_component->setTextInteractionFlags(Qt::TextSelectableByMouse);

    generalGrid->setColumnStretch(1, 1);

    m_installedVersionBox = new QGroupBox;
    m_installedVersionBox->setTitle(i18nc("@title:group", "Installed Version"));
    QGridLayout *installedGridLayout = new QGridLayout(m_installedVersionBox);
    m_installedVersionBox->setLayout(installedGridLayout);

    // installedVersionBox, row 0
    QLabel *installedVersionLabel = new QLabel(m_installedVersionBox);
    installedVersionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    installedVersionLabel->setText(i18nc("@label Label preceding the package version", "Version:"));
    m_installedVersion = new QLabel(m_installedVersionBox);
    m_installedVersion->setTextInteractionFlags(Qt::TextSelectableByMouse);
    installedGridLayout->addWidget(installedVersionLabel, 0, 0, Qt::AlignRight);
    installedGridLayout->addWidget(m_installedVersion, 0, 1, Qt::AlignLeft);
    // installedVersionBox, row 1
    QLabel *installedSizeLabel = new QLabel(m_installedVersionBox);
    installedSizeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    installedSizeLabel->setText(i18nc("@label Label preceding the package size", "Installed Size:"));
    m_installedSize = new QLabel(m_installedVersionBox);
    m_installedSize->setTextInteractionFlags(Qt::TextSelectableByMouse);
    installedGridLayout->addWidget(installedSizeLabel, 1, 0, Qt::AlignRight);
    installedGridLayout->addWidget(m_installedSize, 1, 1, Qt::AlignLeft);
    installedGridLayout->setRowStretch(3, 1);
    installedGridLayout->setColumnStretch(1, 1);


    m_currentVersionBox = new QGroupBox;
    m_currentVersionBox->setTitle(i18nc("@title:group", "Available Version"));
    QGridLayout *currentGridLayout = new QGridLayout(m_currentVersionBox);
    m_currentVersionBox->setLayout(currentGridLayout);

    // currentVersionBox, row 0
    QLabel *currentVersionLabel = new QLabel(m_currentVersionBox);
    currentVersionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    currentVersionLabel->setText(i18nc("@label Label preceding the package version", "Version:"));
    m_currentVersion = new QLabel(m_currentVersionBox);
    m_currentVersion->setTextInteractionFlags(Qt::TextSelectableByMouse);
    currentGridLayout->addWidget(currentVersionLabel, 0, 0, Qt::AlignRight);
    currentGridLayout->addWidget(m_currentVersion, 0, 1, Qt::AlignLeft);
    // currentVersionBox, row 1
    QLabel *currentSizeLabel = new QLabel(m_currentVersionBox);
    currentSizeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    currentSizeLabel->setText(i18nc("@label Label preceding the package size", "Installed Size:"));
    m_currentSize = new QLabel(m_currentVersionBox);
    m_currentSize->setTextInteractionFlags(Qt::TextSelectableByMouse);
    currentGridLayout->addWidget(currentSizeLabel, 1, 0, Qt::AlignRight);
    currentGridLayout->addWidget(m_currentSize, 1, 1, Qt::AlignLeft);
    // currentVersionBox, row 2
    QLabel *downloadSizeLabel = new QLabel(m_currentVersionBox);
    downloadSizeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    downloadSizeLabel->setText(i18nc("@label Label preceding the package's download size", "Download Size:"));
    m_downloadSize = new QLabel(m_currentVersionBox);
    m_downloadSize->setTextInteractionFlags(Qt::TextSelectableByMouse);
    currentGridLayout->addWidget(downloadSizeLabel, 2, 0, Qt::AlignRight);
    currentGridLayout->addWidget(m_downloadSize, 2, 1, Qt::AlignLeft);

    currentGridLayout->setColumnStretch(1, 1);

    QHBoxLayout *versionLayout = new QHBoxLayout;
    versionLayout->addWidget(m_installedVersionBox);
    versionLayout->addWidget(m_currentVersionBox);

    QWidget *verticalSpacer = new QWidget;
    verticalSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalWidget);
    mainLayout->addLayout(versionLayout);
    mainLayout->addWidget(verticalSpacer);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);
    scrollArea->setWidget(mainWidget);
}

void TechnicalDetailsTab::refresh()
{
    if (!m_package) {
        return; // Nothing to refresh yet, so return, else we crash
    }

    m_maintainer->setText(m_package->maintainer());
    m_section->setText(MuonStrings::global()->groupName(m_package->section()));
    m_sourcePackage->setText(m_package->sourcePackage());
    m_origin->setText(m_backend->originLabel(m_package->origin()));
    m_component->setText(m_package->component());

    if (m_package->isInstalled()) {
        m_installedVersionBox->show();
        m_installedVersion->setText(m_package->installedVersion());
        m_installedSize->setText(KFormat().formatByteSize(m_package->currentInstalledSize()));
    } else {
        m_installedVersionBox->hide();
    }

    m_currentVersion->setText(m_package->availableVersion());
    m_currentSize->setText(KFormat().formatByteSize(m_package->availableInstalledSize()));
    m_downloadSize->setText(KFormat().formatByteSize(m_package->downloadSize()));
}

#include "moc_TechnicalDetailsTab.cpp"
