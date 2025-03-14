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

#ifndef MANAGERSETTINGSDIALOG_H
#define MANAGERSETTINGSDIALOG_H

#include <QtCore/QSet>

#include <QDialog>

class SettingsPageBase;
class QAbstractButton;
class QTabWidget;
class QDialogButtonBox;

namespace QApt {
    class Config;
}

class ManagerSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    ManagerSettingsDialog(QWidget *parent, QApt::Config *aptConfig);
    virtual ~ManagerSettingsDialog();

private Q_SLOTS:
    void slotButtonClicked(QAbstractButton* button);
    void changed();
    void authChanged();

Q_SIGNALS:
    void settingsChanged();

private:
    QApt::Config *m_aptConfig;

    void applySettings();
    void restoreDefaults();

private:
    QTabWidget *m_tabWidget;
    QDialogButtonBox *m_buttonBox;
    QSet<SettingsPageBase*> m_pages;
};

#endif
