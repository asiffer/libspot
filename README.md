# libspot [![snap status](https://snapcraft.io/libspot/badge.svg)](https://snapcraft.io/libspot)

## Introduction
**libspot** is a `C++` library which aims to detect anomalies in univariate streaming data. 

Github pages : https://asiffer.github.io/libspot/

## Install

### Latest release

Automatic releases provide compiled libraries for linux/amd64 platforms.
Check the assets of the [latest version](https://github.com/asiffer/libspot/releases).

### From source

Otherwise, you can clone the repository and install it manually:
```shell
git clone https://github.com/asiffer/libspot.git
make
sudo make install
```

The default paths are `/usr/include/` for the headers and `/usr/lib/` for the library.
You can change these paths in the installation step:

```shell
make install INSTALL_HEAD_DIR=... INSTALL_LIB_DIR=...
```

After that, you can test the `SPOT` algorithm with `make test`.

### Snap package

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-white.svg)](https://snapcraft.io/libspot)

Currently, you should use the **edge** version:
```shell
sudo snap install libspot --edge
```

### Debian package

Basically, you need to add the ppa and download the packages you want 
(`libspot` or `libspot-dev` if you want the headers)

```shell
add-apt-repository ppa:asiffer/libspot
apt-get update
apt-get install libspot libspot-dev
```

## Get started

TODO

## Python

Yes, you could interact with the library with python bindings (see https://asiffer.github.io/libspot/python/).

## About 

This work is based on the following publication:

*Siffer, A., Fouque, P. A., Termier, A., & Largouet, C. (2017, August). Anomaly Detection in Streams with Extreme Value Theory. In Proceedings of the 23rd ACM SIGKDD International Conference on Knowledge Discovery and Data Mining (pp. 1067-1075). ACM.*