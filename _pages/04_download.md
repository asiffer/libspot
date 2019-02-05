---
layout: default
title: Download
permalink: download/
categories: 
  - main
---

### Git
The code is available on github. You can either clone the repository

```shell
$ git clone https://github.com/asiffer/libspot.git
```
or download the last version archive <a href="https://github.com/asiffer/libspot/archive/master.zip">here</a>.


### Ubuntu
For Ubuntu users, the following personal package archive (ppa) can be added to you system:

```shell
$ sudo add-apt-repository ppa:asiffer/libspot
$ sudo apt-get update
```

Then you can easily install the `libspot` package (and the `libspot-dev` package if you want the headers):

```shell
$ sudo apt install libspot libspot-dev 
```

### Debian-like distributions
As the packaging is made on launchpad, they are specific to Ubuntu. However, the .deb files can easily be added to other debian-like systems.
In `/etc/apt/sources.list`:
```ini
...
deb http://ppa.launchpad.net/asiffer/libspot/ubuntu bionic main
```
Then you can add the public key to your system and download the package:
```shell
$ curl https://keyserver.ubuntu.com/pks/lookup\?op\=get\&search\=0x76F580E5B58944D9 | sudo apt-key add -
$ sudo apt update
$ sudo apt install libspot libspot-dev 
```


### Python

The `python3` bindings are also available on <a href="https://github.com/asiffer/python3-libspot">github</a> or through the same ppa:

```shell
$ sudo apt install python3-libspot
```

### Golang

The `Go` bindings have also been implemented. You can get it through:
```shell
$ go get github.com/asiffer/gospot
```