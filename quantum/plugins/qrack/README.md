The base repository of the Qrack framework is available here:
https://github.com/vm6502q/qrack

See that repository's README and https://qrack.readthedocs.io/en/latest/ for detailed information about Qrack and getting started.

Qrack can be installed, to be made available for XACC use.
From the root of the Qrack repository directory, run these commands:

```
mkdir _build && cd _build && cmake .. && make all install
```

XACC looks for the Qrack library with "find_library(NAMES qrack)," so it will find it for example as "libqrack.a" in /usr/local/lib.
If your installation is a non-system-standard path, "CMAKE_PREFIX_PATH" should tell CMake where it can find the library.

Qrack OpenCL kernels can be precompiled through the "qrack_cl_precompile" command line utility in the base Qrack library.
When switching between float and double build accuracy, you MUST recompile (or delete) any precompiled kernels.
