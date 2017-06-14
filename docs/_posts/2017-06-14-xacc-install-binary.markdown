---
layout: post
title: XACC Install Pre-built Binaries
permalink: /build/xacc_install
category: build
---

# XACC Install Pre-Built Binaries


## Fedora 25 Install Instructions

```bash
# For Scaffold support... (Required as of 2017-06-14)
$ dnf install -y https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm 

# Install XACC 
$ dnf install -y http://eclipseice.ornl.gov/downloads/xacc/rpms/x86_64/xacc-1.0-1.fc25.x86_64.rpm
```

## Ubuntu 16.04 Install Instructions

```bash
# Install boilerplate 
$ apt-get -y upgrade && apt-get -y update && apt-get install -y software-properties-common wget && add-apt-repository ppa:ubuntu-toolchain-r/test

# Pull down XACC and Scaffold
$ wget http://eclipseice.ornl.gov/downloads/xacc/debs/xacc_1.0_amd64.deb 
$ wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb

# Install Scaffold Dependencies 
$ apt-get -y update && apt-get -y install $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g') 

# Install XACC Dependencies
$ apt-get -y update && apt-get install -y $(dpkg --info xacc_1.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g')

# Install Scaffold and XACC
$ dpkg -i scaffold_2.0_amd64.deb && dpkg -i xacc_1.0_amd64.deb 
```
