## Building with mlpack

To build the mlpack optimizer plugins, you must pull down ensmallen and armadillo

```bash
$ mkdir $HOME/.xacc_tpls
$ cd $HOME/.xacc_tpls
$ git clone https://github.com/mlpack/ensmallen
$ git clone https://gitlab.com/conradsnicta/armadillo-code
```

Now configure XACC, pointing to the appropriate ensmallen and armadillo-code installs

```bash
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc && mkdir build && cd build
$ cmake .. -DXACC_ENSMALLEN_INCLUDE_DIR=$HOME/.xacc_tpls/ensmallen/include
           -DXACC_ARMADILLO_INCLUDE_DIR=$HOME/.xacc_tpls/armadillo/include
$ make -j4 install
```
That should install the mlpack optimizer!