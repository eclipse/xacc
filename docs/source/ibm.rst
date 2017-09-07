IBM Integration
================

Installation
-------------

.. note::

   If you want support for the D-Wave Accelerator, you must install
   `CppRestSDK <https://github.com/microsoft/cpprestsdk>`_ and OpenSSL. This
   is required for these Accelerators to make remote HTTP Rest calls to their
   respective server APIs. Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) brew install cpprestsdk
      $ (fedora) dnf install cpprest-devel openssl-devel
      $ (ubuntu) apt-get install libcpprest-dev libssl-dev


The `IBM Plugin <https://github.com/ornl-qci/xacc-ibm>`_ provides
support to XACC for executing programs
on the IBM Quantum Experience via the IBM Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-ibm

You have now installed the IBM plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerator``,
where ``XACC_ROOT`` is your XACC install prefix.
