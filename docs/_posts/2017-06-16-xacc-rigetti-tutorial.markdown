---
layout: post
title: XACC Rigetti Accelerator Tutorial
permalink: /tutorials/rigetti
category: tutorials
---

## Using XACC to Execute Quantum Programs on the Rigetti QPU

XACC can now execute quantum kernels on the Rigetti QVM. To learn more about 
the architecture of this support, check out 
[(RigettiAccelerator)](https://ornl-qci.github.io/xacc/accelerators/rigetti).

This post provides a tutorial on how to actually use this support. 

# Prerequisites

To run this tutorial, you need to install OpenSSL on your machine. To do so 
on Fedora: 

```bash
dnf install -y openssl openssl-devel
```

On Ubuntu: 

```bash
apt-get install -y libssl-dev
```

Furthermore, to execute on the Rigetti QVM, you must have been provided 
a valid Rigetti API key. Check out [Forest](forest.rigetti.com) to learn how 
to get this key. 

# XACC-Rigetti Quantum Teleportation 

To install XACC, run the following (note here we are using a Fedora 25 Linux install):

```bash
dnf install -y openssl-devel https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm \
    http://eclipseice.ornl.gov/downloads/xacc/rpms/x86_64/xacc-1.0-1.fc25.x86_64.rpm

```
