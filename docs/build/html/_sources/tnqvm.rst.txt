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

   $ xacc-install-plugins.py -p tnqvm

You have now installed the TNQVM plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerator``, where ``XACC_ROOT`` is your XACC install prefix.

