[![Build Status](http://ci.eclipse.org/xacc/buildStatus/icon?job=xacc-ci)](http://ci.eclipse.org/xacc/job/xacc-ci/)

# Eclipse XACC 
## Hardware Agnostic Quantum Programming 

XACC is a programming framework for extreme-scale, post-exascale accelerator architectures that integrates alongside existing conventional applications. It is a pluggable framework for programming languages and hardware developed for next-gen computing hardware architectures like quantum and neuromorphic computing. It lets computational scientists efficiently offload work to attached accelerators through user-friendly Kernel definitions. XACC makes post-exascale hybrid programming approachable for domain computational scientists.

XACC currently supports hybrid classical-quantum programming and enables the execution of quantum kernels on IBM, Rigetti, and D-Wave QPUs.

Installation
------------
Users can choose a couple ways to install the framework - using python/pip 
```bash
$ python -m pip install --user .
```
or cmake/make
```bash
$ mkdir build && cd build
$ cmake .. (without Python support)
$ cmake .. -DPYTHON_INCLUDE_DIR=$(python -c "import sysconfig; print(sysconfig.get_paths()['platinclude'])") (with Python support)
$ make install
```

Documentation
-------------

* [Website and Documentation ](https://xacc.readthedocs.io)

Questions, Bug Reporting, and Issue Tracking
--------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a new issue with the bug tag. You can ask
questions by creating a new issue with the question tag.

License
-------

XACC is dual licensed - [Eclipse Public License](LICENSE.EPL) and [Eclipse Distribution License](LICENSE.EDL).
