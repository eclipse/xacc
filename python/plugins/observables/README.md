## Install Psi4

```bash
$ git clone https://github.com/psi4/psi4 && cd psi4 && mkdir build && cd build
$ cmake .. -DPYTHON_EXECUTABLE=$(which python3.7) -DCMAKE_INSTALL_PREFIX=$(python3.7 -m site --user-site)/psi4
$ make -j24 install
$ export PYTHONPATH=~/.xacc:~/.local/lib/python3.7/site-packages/psi4/lib
```
## Install PySCF
```bash
$ pip install pyscf
```
