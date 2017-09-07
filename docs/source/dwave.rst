D-Wave Integration
===================

Installation
------------

.. note::

   If you want support for the D-Wave Accelerator, you must install
   `CppRestSDK <https://github.com/microsoft/cpprestsdk>`_ and OpenSSL. This
   is required for these Accelerators to make remote HTTP Rest calls to their
   respective server APIs. Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) brew install cpprestsdk
      $ (fedora) dnf install cpprest-devel openssl-devel
      $ (ubuntu) apt-get install libcpprest-dev libssl-dev

The `D-Wave Plugin <https://github.com/ornl-qci/xacc-dwave>`_ provides
support to XACC for executing programs on the D-Wave QPU via the D-Wave Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-dwave

You have now installed the D-Wave plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerator`` and ``$XACC_ROOT/lib/plugins/compilers``, where XACC_ROOT is your XACC install prefix.

Extensibility for Minor Graph Embedding Algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
XACC has extensibility built in for minor graph embedding
algorithms. We currently have one supported embedding algorithm (with more coming online), a wrapper around the D-Wave SAPI Cai, Macready, Roi algorithm. In order to install this as a plugin, run the following

.. note::

   The following embedding algorithm needs to leverage the proprietary
   D-Wave SAPI header file and associated shared library: ``dwave_sapi.h`` and ``libdwave_sapi.so`` (or ``libdwave_sapi.dylib`` on macosx).
   In order for the installation below to work, place dwave_sapi.h in ``/usr/local/include/`` and ``libdwave_sapi.so`` or ``libdwave_sapi.dylib`` in ``/usr/local/lib/``

.. code::

   $ xacc-install-plugins.py -p xacc-dwave-sapi-embedding

You have now installed the D-Wave plugin. It is located in ``$XACC_ROOT/lib/plugins/embedding``, where ``XACC_ROOT`` is your XACC install prefix.

