cmake_minimum_required(VERSION 3.16)

project(muon)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROJECT_VERSION "6.0.0")
set(PROJECT_VERSION_MAJOR 6)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.1.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Concurrent)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED KIO DBusAddons I18n IconThemes XmlGui)
find_package(QApt 4.0.0 REQUIRED)
#find_package(DebconfKDE 1.1.0 REQUIRED)

set(MUON_VERSION ${PROJECT_VERSION})
configure_file(MuonVersion.h.in MuonVersion.h)

add_subdirectory(src)

set_package_properties(QApt PROPERTIES
        DESCRIPTION "Qt wrapper around the libapt-pkg library"
        PURPOSE "Used to support apt-based distribution systems"
        TYPE REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

find_package(KF6I18n CONFIG REQUIRED)
ki18n_install(po)
