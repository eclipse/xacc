IBM Integration
================

Installation
-------------

.. note::

   If you want support for the IBM Accelerator, you must install
   `CppRestSDK <https://github.com/microsoft/cpprestsdk>`_ and OpenSSL. This
   is required to make remote HTTP Rest calls to the IBM Quantum Experience 
   server APIs. Here's how to install these as binaries on various popular platforms:

   .. code::

      $ (macosx) brew install cpprestsdk
      $ (fedora) dnf install cpprest-devel openssl-devel
      $ (ubuntu) apt-get install libcpprest-dev libssl-dev
      $ (slack) slack install cpprestsdk
      $ (slack) slack install openssl


The `IBM Plugin <https://github.com/ornl-qci/xacc-ibm>`_ provides
support to XACC for executing programs
on the IBM Quantum Experience via the IBM Accelerator.

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-ibm

.. note::

   It has been observed on Mac OS X that the above command may fail 
   due to CMake incorrectly finding OpenSSL. If that happens, run the 
   following:
   
   .. code::
      
      $ xacc-install-plugins.py -p xacc-ibm -a OPENSSL_ROOT_DIR=/usr/local/opt/openssl

   Homebrew creates the above OpenSSL root directory. If yours is different, then 
   set the ``OPENSSL_ROOT_DIR`` as such. 

You have now installed the IBM plugin. It is located in ``$XACC_ROOT/lib/plugins/accelerator``,
where ``XACC_ROOT`` is your XACC install prefix.
