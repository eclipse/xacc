# xacc-python-plugin

XACC Python plugin installer for benchmark and decorator algorithms

A simple project and script to install XACC Python plugins to the XACC plugin directory

To install a plugin package:
```bash
$ python3 manage.py -i name_of_package
```

To print the lists of available packages to install:
```bash
$ python3 manage.py --list
```

Adding New Plugin Packages
--------------------------
The project is divided into master directories and plugin package directories.
The plugin packages are found within the master directories.

The master directories must contain an `install.ini` file.
The `install.ini` files are structured as
```bash
name_of_package: /{dir_of_package}
```

A new master directory can be added to the installer by editing the `MASTER_DIRS` field in `manage.py`.

If the XACC Python plugin directory exists somewhere besides `~/.xacc/py-plugins`, the
`XACC_PYTHON_PLUGIN_PATH` field in `manage.py` should be edited.

Within the plugin package directories are the XACC plugins (`.py`) that get installed.
If the plugin package has multiple directories, another `install.ini` file is required in that package and
must be structured in the same manner as before.

If the plugin package does not have extra directories, nothing else needs to be done.
