add_subdirectory(icons)

set(muon_SRCS
    main.cpp
    MainWindow.cpp
    ManagerWidget.cpp
    ReviewWidget.cpp
    DetailsTabs/DetailsTab.cpp
    DetailsTabs/MainTab.cpp
    DetailsTabs/ChangelogTab.cpp
    DetailsTabs/DependsTab.cpp
    DetailsTabs/InstalledFilesTab.cpp
    DetailsTabs/TechnicalDetailsTab.cpp
    DetailsTabs/VersionTab.cpp
    DetailsTabs/ElidedLabel.cpp
    DetailsTabs/ElidedLabel.h
    DetailsWidget.cpp
    DownloadModel/DownloadModel.cpp
    DownloadModel/DownloadDelegate.cpp
    FilterWidget/ArchitectureFilter.cpp
    FilterWidget/CategoryFilter.cpp
    FilterWidget/FilterModel.cpp
    FilterWidget/FilterWidget.cpp
    FilterWidget/OriginFilter.cpp
    FilterWidget/StatusFilter.cpp
    PackageModel/PackageModel.cpp
    PackageModel/PackageProxyModel.cpp
    PackageModel/PackageView.cpp
    PackageModel/PackageViewHeader.cpp
    PackageModel/PackageDelegate.cpp
    PackageModel/PackageWidget.cpp
    StatusWidget.cpp
    TransactionWidget.cpp
    config/ManagerSettingsDialog.cpp
    config/GeneralSettingsPage.cpp
    settings/SettingsPageBase.cpp

    muonapt/ChangesDialog.cpp
    muonapt/MuonStrings.cpp
    muonapt/QAptActions.cpp
    muonapt/HistoryView/HistoryView.h
    muonapt/HistoryView/HistoryProxyModel.h
    muonapt/HistoryView/HistoryView.cpp
    muonapt/HistoryView/HistoryProxyModel.cpp

)

ADD_DEFINITIONS(-DLIBEXECDIR="${KDE_INSTALL_LIBEXECDIR_KF}")

kconfig_add_kcfg_files(muon_SRCS GENERATE_MOC config/MuonSettings.kcfgc)
install(FILES config/muon.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})

add_executable(muon ${muon_SRCS})

target_compile_definitions(muon PRIVATE -DCMAKE_INSTALL_FULL_LIBEXECDIR_KF6=\"${KDE_INSTALL_LIBEXECDIR}\")

target_link_libraries(muon #DebconfKDE::Main
                           KF6::KIOWidgets
                           KF6::DBusAddons
                           KF6::I18n
                           KF6::IconThemes
                           KF6::XmlGui
                           Qt6::Concurrent
                           QApt::Main
)

install(TARGETS muon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.muon.desktop  DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES muonui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUIDIR}/muon )
install( FILES org.kde.muon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
