## Building with mlpack

To build the mlpack optimizer plugins, you must pull down ensmallen and armadillo

```bash
$ mkdir $HOME/.xacc_tpls
$ cd $HOME/.xacc_tpls
$ git clone https://github.com/mlpack/ensmallen
$ git clone https://gitlab.com/conradsnicta/armadillo-code armadillo
```
Note on the Rigetti QMI, you may have to download a tar.gz of the armadillo repo and upload via `scp`.
I have found that I am unable to git clone from non-github repos...

Now configure XACC, pointing to the appropriate ensmallen and armadillo-code installs

```bash
$ cd $HOME
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc && mkdir build && cd build
$ cmake .. -DXACC_ENSMALLEN_INCLUDE_DIR=$HOME/.xacc_tpls/ensmallen/include
           -DXACC_ARMADILLO_INCLUDE_DIR=$HOME/.xacc_tpls/armadillo/include
  [add any other cmake args you may need, see top-level readme]
$ make -j4 install
```
That should install the mlpack optimizer!