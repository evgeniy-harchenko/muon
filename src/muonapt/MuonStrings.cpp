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

#include "MuonStrings.h"

#include <KLocalizedString>
#include <QDebug>

#include <QApt/Transaction>

Q_GLOBAL_STATIC_WITH_ARGS(MuonStrings, globalMuonStrings, (0))

using namespace QApt;

MuonStrings *MuonStrings::global()
{
    return globalMuonStrings;
}

MuonStrings::MuonStrings(QObject *parent)
    : QObject(parent)
    , m_groupHash(groupHash())
    , m_stateHash(stateHash())
    , m_archHash(archHash())
{
}

QHash<QString, QString> MuonStrings::groupHash()
{
    QHash<QString, QString> hash;
    hash[QStringLiteral("admin")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"admin\"",
                          "System Administration");
    hash[QStringLiteral("base")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"base\"",
                         "Base System");
    hash[QStringLiteral("cli-mono")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"cli-mono\"",
                             "Mono/CLI Infrastructure");
    hash[QStringLiteral("comm")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"comm\"",
                         "Communication");
    hash[QStringLiteral("database")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"database\"",
                             "Databases");
    hash[QStringLiteral("devel")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"devel\"",
                          "Development");
    hash[QStringLiteral("doc")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"doc\"",
                        "Documentation");
    hash[QStringLiteral("debug")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"debug\"",
                          "Debug");
    hash[QStringLiteral("editors")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"editors\"",
                            "Editors");
    hash[QStringLiteral("electronics")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"electronics\"",
                                "Electronics");
    hash[QStringLiteral("embedded")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"embedded\"",
                             "Embedded Devices");
    hash[QStringLiteral("fonts")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"fonts\"",
                          "Fonts");
    hash[QStringLiteral("games")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"games\"",
                          "Games and Amusement");
    hash[QStringLiteral("gnome")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"gnome\"",
                          "GNOME Desktop Environment");
    hash[QStringLiteral("graphics")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"graphics\"",
                             "Graphics");
    hash[QStringLiteral("gnu-r")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"gnu-r\"",
                          "GNU R Statistical System");
    hash[QStringLiteral("gnustep")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"gnustep\"",
                            "Gnustep Desktop Environment");
    hash[QStringLiteral("hamradio")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"hamradio\"",
                             "Amateur Radio");
    hash[QStringLiteral("haskell")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"haskell\"",
                            "Haskell Programming Language");
    hash[QStringLiteral("httpd")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"httpd\"",
                          "Web Servers");
    hash[QStringLiteral("interpreters")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"interpreters\"",
                                 "Interpreted Computer Languages");
    hash[QStringLiteral("java")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"java\"",
                         "Java Programming Language");
    hash[QStringLiteral("kde")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"kde\"",
                        "KDE Software Compilation");
    hash[QStringLiteral("kernel")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"kernel\"",
                           "Kernel and Modules");
    hash[QStringLiteral("libdevel")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"libdevel\"",
                             "Libraries - Development");
    hash[QStringLiteral("libs")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"libs\"",
                         "Libraries");
    hash[QStringLiteral("lisp")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"lisp\"",
                         "Lisp Programming Language");
    hash[QStringLiteral("localization")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"localization\"",
                                 "Localization");
    hash[QStringLiteral("mail")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"mail\"",
                         "Email");
    hash[QStringLiteral("math")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"math\"",
                         "Mathematics");
    hash[QStringLiteral("misc")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"misc\"",
                         "Miscellaneous - Text-based");
    hash[QStringLiteral("net")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"net\"",
                        "Networking");
    hash[QStringLiteral("news")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"news\"",
                         "Newsgroups");
    hash[QStringLiteral("ocaml")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"ocaml\"",
                          "OCaml Programming Language");
    hash[QStringLiteral("oldlibs")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"oldlibs\"",
                            "Libraries - Old");
    hash[QStringLiteral("otherosfs")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"otherosfs\"",
                              "Cross Platform");
    hash[QStringLiteral("perl")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"perl\"",
                         "Perl Programming Language");
    hash[QStringLiteral("php")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"php\"",
                        "PHP Programming Language");
    hash[QStringLiteral("python")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"python\"",
                           "Python Programming Language");
    hash[QStringLiteral("ruby")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"ruby\"",
                         "Ruby Programming Language");
    hash[QStringLiteral("science")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"science\"",
                            "Science");
    hash[QStringLiteral("shells")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"shells\"",
                           "Shells");
    hash[QStringLiteral("sound")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"sound\"",
                          "Multimedia");
    hash[QStringLiteral("tex")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"tex\"",
                        "TeX Authoring");
    hash[QStringLiteral("text")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"text\"",
                         "Word Processing");
    hash[QStringLiteral("utils")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"utils\"",
                          "Utilities");
    hash[QStringLiteral("vcs")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"vcs\"",
                        "Version Control Systems");
    hash[QStringLiteral("video")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"video\"",
                          "Video Software");
    hash[QStringLiteral("web")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"web\"",
                        "Internet");
    hash[QStringLiteral("x11")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"x11\"",
                        "Miscellaneous - Graphical");
    hash[QStringLiteral("xfce")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"xfce\"",
                         "Xfce Desktop Environment");
    hash[QStringLiteral("zope")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"zope\"",
                         "Zope/Plone Environment");
    hash[QStringLiteral("unknown")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"unknown\"",
                            "Unknown");
    hash[QStringLiteral("alien")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"alien\"",
                          "Converted from RPM by Alien");
    hash[QStringLiteral("translations")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"translations\"",
                                 "Internationalization and Localization");
    hash[QStringLiteral("metapackages")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"metapackages\"",
                                 "Meta Packages");
    hash[QStringLiteral("non-us")] = i18nc("@item:inlistbox Debian package section \"non-US\", for packages that cannot be shipped in the US",
                           "Restricted On Export");
    hash[QStringLiteral("non-free")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"non-free\"",
                             "Non-free");
    hash[QStringLiteral("contrib")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"contrib\"",
                            "Contrib");
    hash[QStringLiteral("education")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"education\"",
                            "Education");
    hash[QStringLiteral("introspection")] = i18nc("@item:inlistbox Human-readable name for the Debian package section \"introspection\"",
                            "GObject Introspection Data");
    return hash;
}

QString MuonStrings::groupName(const QString &name) const
{
    QString suffix;

    if (name.contains(QChar::fromLatin1('/'))) {
        QStringList split = name.split(QChar::fromLatin1('/'));
        suffix = split.at(1);

        return m_groupHash.value(suffix);
    } else {
        return m_groupHash.value(name);
    }
}

QString MuonStrings::groupKey(const QString &text) const
{
    return m_groupHash.key(text);
}

QHash<int, QString> MuonStrings::stateHash()
{
    QHash<int, QString> hash;
    hash[Package::ToKeep] = i18nc("@info:status Package state", "No Change");
    hash[Package::ToInstall] = i18nc("@info:status Requested action", "Install");
    hash[Package::NewInstall] = i18nc("@info:status Requested action", "Install");
    hash[Package::ToReInstall] = i18nc("@info:status Requested action", "Reinstall");
    hash[Package::ToUpgrade] = i18nc("@info:status Requested action", "Upgrade");
    hash[Package::ToDowngrade] = i18nc("@info:status Requested action", "Downgrade");
    hash[Package::ToRemove] = i18nc("@info:status Requested action", "Remove");
    hash[Package::Held] = i18nc("@info:status Package state" , "Held");
    hash[Package::Installed] = i18nc("@info:status Package state", "Installed");
    hash[Package::Upgradeable] = i18nc("@info:status Package state", "Upgradeable");
    hash[Package::NowBroken] = i18nc("@info:status Package state", "Broken");
    hash[Package::InstallBroken] = i18nc("@info:status Package state", "Install Broken");
    hash[Package::Orphaned] = i18nc("@info:status Package state", "Orphaned");
    hash[Package::Pinned] = i18nc("@info:status Package state", "Locked");
    hash[Package::New] = i18nc("@info:status Package state", "New in repository");
    hash[Package::ResidualConfig] = i18nc("@info:status Package state", "Residual Configuration");
    hash[Package::NotDownloadable] = i18nc("@info:status Package state", "Not Downloadable");
    hash[Package::ToPurge] = i18nc("@info:status Requested action", "Purge");
    hash[Package::IsImportant] = i18nc("@info:status Package state", "Important for base install");
    hash[Package::OverrideVersion] = i18nc("@info:status Package state", "Version overridden");
    hash[Package::IsAuto] = i18nc("@info:status Package state", "Required by other packages");
    hash[Package::IsGarbage] = i18nc("@info:status Package state", "Installed (auto-removable)");
    hash[Package::NowPolicyBroken] = i18nc("@info:status Package state", "Policy Broken");
    hash[Package::InstallPolicyBroken] = i18nc("@info:status Package state", "Policy Broken");
    hash[Package::NotInstalled] = i18nc("@info:status Package state" , "Not Installed");
    hash[Package::IsPinned] = i18nc("@info:status Package locked at a certain version",
                                          "Locked");
    hash[Package::IsManuallyHeld] = i18nc("@info:status Package state", "Manually held back");

    return hash;
}

QString MuonStrings::packageStateName(Package::State state) const
{
    return m_stateHash.value(state);
}

QString MuonStrings::packageChangeStateName(Package::State state) const
{
    int ns = state & (Package::ToKeep | Package::ToInstall | Package::ToReInstall | Package::NewInstall
                                    | Package::ToUpgrade | Package::ToRemove
                                    | Package::ToPurge | Package::ToDowngrade);
    return m_stateHash.value(ns);
}

QHash<QString, QString> MuonStrings::archHash()
{
    QHash<QString, QString> hash;
    hash[QStringLiteral("all")] = i18nc("@item:inlistbox", "Common");
    hash[QStringLiteral("i386")] = i18nc("@item:inlistbox CPU architecture", "32-bit");
    hash[QStringLiteral("amd64")] = i18nc("@item:inlistbox CPU architecture", "64-bit");
    hash[QStringLiteral("powerpc")] = i18nc("@item:inlistbox PU architecture", "Power PC");

    return hash;
}

QString MuonStrings::archString(const QString &arch) const
{
    QString str = m_archHash.value(arch);

    if (str.isEmpty())
        str = arch;

    return str;
}

QString MuonStrings::errorTitle(ErrorCode error) const
{
    switch (error) {
    case InitError:
        return i18nc("@title:window", "Initialization Error");
    case LockError:
        return i18nc("@title:window", "Unable to Obtain Package System Lock");
    case DiskSpaceError:
        return i18nc("@title:window", "Low Disk Space");
    case FetchError:
    case CommitError:
        return i18nc("@title:window", "Failed to Apply Changes");
    case AuthError:
        return i18nc("@title:window", "Authentication error");
    case WorkerDisappeared:
        return i18nc("@title:window", "Unexpected Error");
    case UntrustedError:
        return i18nc("@title:window", "Untrusted Packages");
    case UnknownError:
    default:
        return i18nc("@title:window", "Unknown Error");
    }
}

QString MuonStrings::errorText(ErrorCode error, Transaction *trans) const
{
    QString text;

    switch (error) {
    case InitError:
        text = i18nc("@label", "The package system could not be initialized, your "
                               "configuration may be broken.");
        break;
    case LockError:
        text = i18nc("@label",
                     "Another application seems to be using the package "
                     "system at this time. You must close all other package "
                     "managers before you will be able to install or remove "
                     "any packages.");
        break;
    case DiskSpaceError:
        text = i18nc("@label",
                     "You do not have enough disk space in the directory "
                     "at %1 to continue with this operation.", trans->errorDetails());
        break;
    case FetchError:
        text = i18nc("@label", "Could not download packages");
        break;
    case CommitError:
        text = i18nc("@label", "An error occurred while applying changes:");
        break;
    case AuthError:
        text = i18nc("@label",
                     "This operation cannot continue since proper "
                     "authorization was not provided");
        break;
    case WorkerDisappeared:
        text = i18nc("@label", "It appears that the QApt worker has either crashed "
                     "or disappeared. Please report a bug to the QApt maintainers");
        break;
    case UntrustedError:
        text = i18ncp("@label",
                      "The following package has not been verified by its author. "
                      "Downloading untrusted packages has been disallowed "
                      "by your current configuration.",
                      "The following packages have not been verified by "
                      "their authors. "
                      "Downloading untrusted packages has "
                      "been disallowed by your current configuration.",
                      trans->untrustedPackages().size());
        break;
    default:
        break;
    }

    return text;
}
