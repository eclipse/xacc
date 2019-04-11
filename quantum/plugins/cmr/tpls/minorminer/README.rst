.. image:: https://circleci.com/gh/dwavesystems/minorminer.svg?style=svg
    :target: https://circleci.com/gh/dwavesystems/minorminer

.. image:: https://ci.appveyor.com/api/projects/status/g4bj8w7mm1v95i6a/branch/master?svg=true
    :target: https://ci.appveyor.com/project/dwave-adtt/minorminer/branch/master

.. image:: https://coveralls.io/repos/github/dwavesystems/minorminer/badge.svg?branch=master
    :target: https://coveralls.io/github/dwavesystems/minorminer?branch=master

.. image:: http://readthedocs.org/projects/minorminer/badge/?version=latest
    :target: http://minorminer.readthedocs.io/en/latest/?badge=latest

.. index-start-marker

==========
minorminer
==========

`minorminer` is a heuristic tool for minor embedding: given a minor and target graph, it tries to find a mapping that embeds the minor into the target.

The primary utility function, ```find_embedding()```, is an implementation of the heuristic algorithm described in [1]. It accepts various optional parameters used to tune the algorithm's execution or constrain the given problem.

This implementation performs on par with tuned, non-configurable implementations while
providing users with hooks to easily use the code as a basic building block in research.

[1] https://arxiv.org/abs/1406.2741

.. index-end-marker

Python
======

Installation
------------

.. install-python-start

pip installation is recommended for platforms with precompiled wheels posted to pypi. Source distributions are provided as well.

.. code-block:: bash

    pip install minorminer

To install from this repository, run the `setuptools` script.

.. code-block:: bash

    pip install cython==0.27
    python setup.py install
    # optionally, run the tests to check your build
    pip install -r tests/requirements.txt
    python -m nose . --exe


.. install-python-end

Examples
--------

.. code-block:: python

    from minorminer import find_embedding

    # A triangle is a minor of a square.
    triangle = [(0, 1), (1, 2), (2, 0)]
    square = [(0, 1), (1, 2), (2, 3), (3, 0)]

    # Find an assignment of sets of square variables to the triangle variables
    embedding = find_embedding(triangle, square, random_seed=10)
    print(len(embedding))  # 3, one set for each variable in the triangle
    print(embedding)
    # We don't know which variables will be assigned where, here are a
    # couple possible outputs:
    # [[0, 1], [2], [3]]
    # [[3], [1, 0], [2]]

.. code-block:: python

    # We can insist that variable 0 of the triangle will always be assigned to [2]
    embedding = find_embedding(triangle, square, fixed_chains={0: [2]})
    print(embedding)
    # [[2], [3, 0], [1]]
    # [[2], [1], [0, 3]]
    # And more, but all of them start with [2]

.. code-block:: python

    # If we didn't want to force variable 0 to stay as [2], but we thought that
    # was a good start we could provide it as an initialization hint instead.
    embedding = find_embedding(triangle, square, initial_chains={0: [2]})
    print(embedding)
    # [[2], [0, 3], [1]]
    # [[0], [3], [1, 2]]
    # Output where variable 0 has switched to something else is possible again.

.. code-block:: python

    import networkx as nx

    # An example on some less trivial graphs
    # We will try to embed a fully connected graph with 6 nodes, into a
    # random regular graph with degree 3.
    clique = nx.complete_graph(6).edges()
    target_graph = nx.random_regular_graph(d=3, n=30).edges()

    embedding = find_embedding(clique, target_graph)

    print(embedding)
    # There are many possible outputs for this, sometimes it might even fail
    # and return an empty list

A more fleshed out example can be found under `examples/fourcolor.py`

.. code-block:: bash

    cd examples
    pip install -r requirements.txt
    python fourcolor.py

Matlab
======

Installation
------------

.. install-matlab-start

The mex bindings for this library will work with some versions of 2013 and earlier,
and versions from 2016b an onward. An example build command used in Ubuntu is
found in the makefile `matlab/make.m`.

If you run `make` in the `matlab` directory on Ubuntu it should generate
`find_embedding.mexa64`, which can be added to the MATLAB path.

.. install-matlab-end

Examples
--------

.. code-block:: matlab

    % A triangle is a minor of a square.
    triangle = triu(ones(3),1);
    square = sparse([1,2,3,4],[2,3,4,1],[1,1,1,1],4,4);

    % Find an assignment of sets of square variables to the triangle variables
    options = struct('random_seed',10);
    embedding = find_embedding_matlab_wrapper(triangle, square, options)
    % typically in matlab we use indices starting at one rather than 0:
    embedding = cellfun(@(x)x+1,embedding,'UniformOutput',false);
    embedding{:}

.. code-block:: matlab

    % We can insist that variable 0 of the triangle will always be assigned to
    % [2] (zero-indexed)
    chains = cell(1);
    chains{1} = 2;
    options = struct();
    options.fixed_chains = chains;
    embedding = find_embedding(triangle, square, options)
    embedding{:}

.. code-block:: matlab

    % If we didn't want to force variable 0 to stay as [2], but we thought that
    % was a good start we could provide it as an initialization hint instead.
    options = struct();
    options.initial_chains = chains;
    embedding = find_embedding(triangle, square, options)
    embedding{:}

C++
===

Installation
------------

.. install-c-start

The `CMakeLists.txt` in the root of this repo will build the library and optionally run a series of tests. On linux the commands would be something like this:

.. code-block:: bash

    mkdir build; cd build
    cmake ..
    make

To build the tests turn the cmake option `MINORMINER_BUILD_TESTS` on. The command line option for cmake to do this would be `-DMINORMINER_BUILD_TESTS=ON`.

Library Usage
-------------

C++11 programs should be able to use this as a header-only library. If your project is using CMake this library can be used fairly simply; if you have checked out this repo as `externals/minorminer` in your project you would need to add the following lines to your `CMakeLists.txt`

.. code-block:: CMake

    add_subdirectory(externals/minorminer)

    # After your target is defined
    target_link_libraries(your_target minorminer pthread)

.. install-c-end

Examples
--------

A minimal example that can be built can be found in this repo under `examples/example.cpp`.

.. code-block:: bash

    cd examples
    g++ example.cpp -std=c++11 -o example -pthread

This can also be built using the included `CMakeLists.txt` along with the main library build by turning the cmake option `MINORMINER_BUILD_EXAMPLES` on. The command line option for cmake to do this would be `-DMINORMINER_BUILD_EXAMPLES=ON`.
