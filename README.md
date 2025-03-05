<p align="center">
  <img src="https://github.com/evgeniy-harchenko/muon/blob/master/assets/logo.png" width=150 />
  <h1 align="center">Muon Qt6</h1>
  <p align="center">Manage applications and libraries installed on your system to the package level. Search, install and remove packages and inspect their versions and their dependencies.</center>
</p>

<p align="center">
  <img src="https://github.com/evgeniy-harchenko/muon/blob/master/assets/image.png"/>
</p>

## Build instructions

### Requirements

- Install [QApt >= 4.0.0](https://github.com/evgeniy-harchenko/qapt)

### Build dependencies:
```bash
cmake build-essential extra-cmake-modules qt6-base-dev libkf6kio-dev kf6-kdbusaddons-dev libkf6i18n-dev kf6-kiconthemes-dev kf6-kxmlgui-dev
```
### Runtime dependencies:
```bash
apt-xapian-index software-properties-qt
```
### Install
```bash
cd /where/your/downloaded/muon/source
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```
