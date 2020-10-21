# Using QAOA command line interface
Build XACC from this fork:
```bash
    $ git clone https://github.com/anthony-santana/xacc.git
    $ cd xacc && mkdir build && cd build
    [default cmake call]
    $ cmake ..
    [with tests and examples]
    $ cmake .. -DXACC_BUILD_EXAMPLES=TRUE -DXACC_BUILD_TESTS=TRUE
    [now build xacc]
    $ make install
    [for a speedier build on linux]
    $ make -j$(nproc --all) install
    [and on mac os x]
    $ make -j$(sysctl -n hw.physicalcpu) install
    [if built with tests, run them]
    $ ctest --output-on-failure
```

set your path from the terminal:
```bash
    export PATH=$PATH:$HOME/.xacc/bin
```

Navigate to tools/qaoa/examples for reference on formatting the graph files, as well as the json configuration files. If you'd prefer to run the algorithm without a json file, this may now be done as:

```bash
    xacc-qaoa -i graph_input_file.txt
```

Additional flags may be added:

```bash
    xacc-qaoa -i graph_input_file.txt -p 1 -qpu qpp -opt nlopt 
```

where -p is the number of qaoa steps, -qpu is the simulator/backend, and -opt is the optimizer. The values passed above are the same as the default used when the user doesn't specify. 

If you'd prefer to pass in a json configuration file, this may be done as:

```bash
    xacc-qaoa -c qaoa_config_example.json
```

The example json configuration file outlines all of the parameters that users may control at this time, but only a graph file is required. Future plans include control over the graphs to run and being able to pass multiple graph files in.