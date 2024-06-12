# Muon Qt6

## Build instructions

### Requirements

- Install [QApt >= 4.0.0](https://github.com/evgeniy-harchenko/qapt)

### Build dependencies:
```bash
cmake build-essential extra-cmake-modules qt6-base-dev libkf6kio-dev kf6-kdbusaddons-dev libkf6i18n-dev kf6-kiconthemes-dev kf6-kxmlgui-dev
```
### Runtime dependencies:
```bash
apt-xapian-index kdialog software-properties-qt
```
### Install
```bash
cd /where/your/downloaded/muon/source
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```
