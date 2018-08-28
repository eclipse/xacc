D-Wave
=======

Installation
------------
The `D-Wave Plugin <https://github.com/ornl-qci/xacc-dwave>`_ provides
support to XACC for executing programs on the D-Wave QPU via the D-Wave Accelerator.

To install this plugin, run the following

.. code::

   $ git clone https://github.com/ornl-qci/xacc-dwave
   $ cd xacc-dwave && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc

Alternatively, if you installed the XACC Python bindings, then you can run

.. code::

   $ cmake .. -DXACC_DIR=$(python -m xacc -L)

ensuring that the ``xacc`` module is in your ``PYTHONPATH``.

You have now installed the D-Wave plugin. It is located in ``$XACC_ROOT/plugins``,
where ``XACC_ROOT`` is your XACC install prefix.

Setting Credentials
-------------------

In order to target the D-Wave Qubist remote resources you must provide
XACC with your API key. By default
XACC will search for a config file at ``$HOME/.dwave_config``.

If you installed the XACC Python bindings, then you can run

.. code::

   $ python -m xacc -c dwave -k YOURAPIKEY 
