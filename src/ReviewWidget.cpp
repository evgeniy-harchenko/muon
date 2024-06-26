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

#include "ReviewWidget.h"

#include <KLocalizedString>

#include "MuonSettings.h"

ReviewWidget::ReviewWidget(QWidget *parent)
    : PackageWidget(parent)
{
    setPackagesType(PackageWidget::MarkedPackages);

    setHeaderText(i18n("<b>Review and Apply Changes</b>"));
    restoreColumnsState(QByteArray::fromBase64(MuonSettings::self()->reviewListColumns().toLatin1()));
}

ReviewWidget::~ReviewWidget()
{
    //MuonSettings::self()->setReviewListColumns(saveColumnsState().toBase64());
    MuonSettings::self()->setReviewListColumns(QString::fromUtf8(saveColumnsState()));
    MuonSettings::self()->save();
}

#include "moc_ReviewWidget.cpp"
