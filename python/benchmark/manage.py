# Installation management script for XACC benchmark plugin installation
#
import sys
import argparse
import os
import configparser
from shutil import copy
import xacc
import sysconfig
import importlib
import ast
import subprocess
import inspect

MASTER_DIRS = ['vqe']

MASTER_PACKAGES = {}

REQUIREMENTS = {}

TOP_PATH = os.path.dirname(os.path.realpath(__file__))

PLUGIN_INSTALLATIONS = {}

def parse_args(args):
    parser = argparse.ArgumentParser(description="Installation manager for XACC benchmark plugins.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                                     fromfile_prefix_chars='@')
    parser.add_argument("-i", "--install", type=str, help="Install a plugin package to the XACC plugin directory.", required=False)
    parser.add_argument("-l", "--list", help="List all available plugin packages for installation.", required=False, action='store_true')
    parser.add_argument("-p", "--path", help="Set the XACC Python Plugin path (default = /root/.xacc/py-plugins)", required=False)

    opts = parser.parse_args(args)
    return opts

def install_package(install_name):
    try:
        xacc.info("Retrieving package and checking requirements..")
        package_path = PLUGIN_INSTALLATIONS[install_name]
        for k,v in MASTER_PACKAGES.items():
            if install_name in v and k in REQUIREMENTS:
                requirement = REQUIREMENTS[k]['module']
                mdir = k
                importlib.import_module(requirement)
    except KeyError as ex:
        xacc.info("There is no '{}' XACC Python plugin package available.".format(install_name))
        exit(1)
    # this might have to change as more packages and their requirements get added
    # for now, it works fine, and should work fine for any XACC requirement
    # that needs to be git-cloned and built with cmake-make (vqe)
    except ModuleNotFoundError as ex:
        xacc.info("You do not have the required Python module `{}` to install and run the '{}' XACC benchmark plugin package.".format(requirement, install_name))
        yn = input("Install requirements? (y/n) ")
        if yn == "y":
            dest = os.path.dirname(inspect.getfile(xacc))
            install_path = os.path.join(dest, REQUIREMENTS[mdir]['dir'])
            build_path = os.path.join(install_path, 'build')
            os.chdir(dest)
            subprocess.run(['git', 'clone', '--recursive', '{}'.format(REQUIREMENTS[mdir]['repo'])])
            os.makedirs(build_path)
            os.chdir(build_path)
            subprocess.run(['cmake', '..', '-DXACC_DIR={}'.format(dest), '-DPYTHON_INCLUDE_DIR={}'.format(sysconfig.get_paths()['include'])])
            subprocess.run(['make', '-j2', 'install'])
        else:
            exit(1)

    install_directive = os.path.join(package_path+"/install.ini") if os.path.isfile(package_path+"/install.ini") else None
    plugin_files = []
    if not install_directive:
        plugin_files += [package_path+"/"+f for f in os.listdir(
            package_path) if os.path.isfile(os.path.join(package_path, f)) and f.endswith(".py")]
    else:
        plugin_dict, plugin_list = read_install_directive(install_directive, package_path)
        for k,v in plugin_dict.items():
            mini_package_path = v
            plugin_files += [v+"/"+f for f in os.listdir(v) if os.path.isfile(os.path.join(v, f)) and f.endswith(".py")]
    n_plugins = len(plugin_files)
    for plugin in plugin_files:
        copy(os.path.join(plugin), XACC_PYTHON_PLUGIN_PATH)

    xacc.info("Installed {} plugins from the '{}' package to the {} directory.".format(n_plugins, install_name, XACC_PYTHON_PLUGIN_PATH))

def set_plugin_path(path):
    global XACC_PYTHON_PLUGIN_PATH
    XACC_PYTHON_PLUGIN_PATH = path

def read_install_directive(install_file, parent):
    config = configparser.RawConfigParser()
    config.read(install_file)
    results = {}
    packages_here = []
    for section in config.sections():
        if section == "Requirements":
            for req in config.items(section):
                name, require = req
                REQUIREMENTS[name] = ast.literal_eval(require)
        else:
            for installation in config.items(section):
                name, folder = installation
                packages_here.append(name)
                folder = parent+folder
                results.update(dict([(name, folder)]))
    return results, packages_here

def get_packages():

    for mdir in MASTER_DIRS:
        plugins_path = os.path.join(TOP_PATH, mdir)
        install_directives = plugins_path+"/install.ini"
        package_dict, package_list = read_install_directive(install_directives, plugins_path)
        PLUGIN_INSTALLATIONS.update(package_dict)

        if mdir not in MASTER_PACKAGES:
            MASTER_PACKAGES[mdir] = package_list
        else:
            MASTER_PACKAGES[mdir] += package_list

def main(argv=None):
    opts = parse_args(sys.argv[1:])
    get_packages()
    if opts.path:
        set_plugin_path(opts.path)

    if opts.install:
        install_package(opts.install)

    if opts.list:
        xacc.info("Available XACC Python plugin packages:")
        for k, v in MASTER_PACKAGES.items():
            xacc.info("{:5}: {!s}".format(k, v))

if __name__ == "__main__":
    sys.exit(main())
