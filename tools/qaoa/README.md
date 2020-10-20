# Quick Start with Docker

```bash
$ docker run --security-opt seccomp=unconfined --init -it -p 3000:3000 xacc/xacc
```
Navigate in your browser to localhost:3000, open a terminal in the new IDE and 
follow the instructions in 'Build these examples'.

# Build XACC

Check out the instructions at [here](https://github.com/eclipse/xacc#build-from-source). 

# Build these examples

```bash
$ git clone https://code.ornl.gov/qci/xacc-qaoa-example
$ cd xacc-qaoa-example && mkdir build && cd build
$ cmake .. 
$ make 
$ ./qaoa_maxcut_from_graph
```