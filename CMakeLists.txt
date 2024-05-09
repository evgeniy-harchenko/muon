cmake_minimum_required(VERSION 3.16.0)

project(muon)

set(PROJECT_VERSION "6.0.0")
set(PROJECT_VERSION_MAJOR 6)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.1.0")

find_package(ECM ${KF_MIN_VERSION} NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Concurrent)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED KIO DBusAddons I18n IconThemes XmlGui)

find_package(QApt 3.0.5 REQUIRED)
#find_package(DebconfKDE 1.1.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(MUON_VERSION ${PROJECT_VERSION})
configure_file(MuonVersion.h.in MuonVersion.h)

add_subdirectory(src)

set_package_properties(QApt PROPERTIES
    DESCRIPTION "Qt wrapper around the libapt-pkg library"
    PURPOSE "Used to support apt-based distribution systems"
    TYPE REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
