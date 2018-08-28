XACC-VQE
========

Installation
------------

The `XACC-VQE Plugin <https://github.com/ornl-qci/xacc-vqe>`_ provides
support to XACC for executing programs
on the ORNL tensor network quantum virtual machine.

To install this plugin, run the following

.. code::

   $ git clone https://github.com/ornl-qci/xacc-vqe
   $ cd xacc-vqe && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc

If you installed the XACC Python bindings, then you can run

.. code::

   $ cmake .. -DXACC_DIR=$(python -m xacc -L)

ensuring that xacc is in your ``PYTHONPATH``.

You have now installed the XACC-VQE plugin. It is located in ``$XACC_ROOT/plugins``,
where ``XACC_ROOT`` is your XACC install prefix.
