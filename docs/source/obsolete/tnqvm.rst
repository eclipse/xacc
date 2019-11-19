TNQVM
======

Installation
--------------

The `TNQVM Plugin <https://github.com/ornl-qci/tnqvm>`_ provides
support to XACC for executing programs
on the ORNL tensor network quantum virtual machine.

.. note::

   This Accelerator requires BLAS/LAPACK libraries to be installed.
   Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) should already be there in Accelerate Framework, if not
      $ (macosx) brew install openblas lapack
      $ (fedora) dnf install blas-devel lapack-devel
      $ (ubuntu) apt-get install libblas-dev liblapack-dev

To install this plugin, run the following

.. code::

   $ git clone https://github.com/ornl-qci/tnqvm
   $ cd tnqvm && mkdir build && cd build
   $ cmake .. -DXACC_DIR=$HOME/.xacc

If you installed the XACC Python bindings, then you can run

.. code::

   $ cmake .. -DXACC_DIR=$(python -m xacc -L)

ensuring that xacc is in your ``PYTHONPATH``.

You have now installed the TNQVM plugin. It is located in ``$XACC_ROOT/plugins``,
where ``XACC_ROOT`` is your XACC install prefix.
