Scaffold
=========

Installation
-------------

.. note::

   Due to issues getting `ScaffCC <https://github.com/ornl-qci/ScaffCC>`_ to link correctly with RTTI on Mac OS X, we do not have a binary package installer for Mac OS X. We are open to PRs on this if you can help.

To use the `Scaffold Plugin <https://github.com/ornl-qci/tnqvm>`_ you must have our fork of
Scaffold installed as a binary package. We have builds for Fedora 25/26 and Ubuntu 16.04/17.04. To install

.. code::

   $ (fedora) dnf install https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold-2.0-1.fc25.x86_64.rpm (REPLACE 25 with 26 if on FC26)
   $ (ubuntu) wget https://github.com/ORNL-QCI/ScaffCC/releases/download/v2.0/scaffold_2.0_amd64.deb
   $ (ubuntu) apt-get install -y $(dpkg --info scaffold_2.0_amd64.deb | grep Depends | sed "s/.*ends:\ //" | sed 's/,//g')
   $ (ubuntu) dpkg -i scaffold_2.0_amd64.deb

To install this plugin, run the following

.. code::

   $ xacc-install-plugins.py -p xacc-scaffold

You have now installed the Scaffold plugin. It is located in ``$XACC_ROOT/lib/plugins/compilers``, where ``XACC_ROOT`` is your XACC install prefix.

