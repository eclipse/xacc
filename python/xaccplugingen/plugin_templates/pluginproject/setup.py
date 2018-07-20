#! /usr/bin/env python3

import os
import re
import sys
import sysconfig
import platform
import subprocess
import multiprocessing
import site

from distutils.version import LooseVersion
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install as InstallCommandBase
from setuptools.command.test import test as TestCommand
from shutil import copyfile, copymode
import shutil
import sysconfig

env = os.environ.copy()

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                         out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))

        script_path = os.path.dirname(os.path.realpath(__file__))
        install_prefix = script_path + '/' + self.build_lib + '/xacc' 
        print(dir(self))
        print(self.build_lib)
  
        import pyxacc
        cmake_args = ['-DPYTHON_INCLUDE_DIR=' + sysconfig.get_paths()['platinclude'], 
                      '-DXACC_DIR='+os.path.dirname(os.path.realpath(pyxacc.__file__)),
		      '-DFROM_SETUP_PY=TRUE']
        args = sys.argv[1:]
        if 'install' not in args:
           cmake_args.append('-DCMAKE_INSTALL_PREFIX='+install_prefix) #install_prefix = script_path + '/' + self.build_lib 
        
        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]

        build_args += ['--', '-j'+str(multiprocessing.cpu_count())]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.', '--target', 'install'] + build_args,
                              cwd=self.build_temp)
        print() # Add an empty line for cleaner output

s = setup(
    name='{project_name}',
    version='0.1',
    author='',
    install_requires=['xacc >= 0.1.2'],
    author_email='',
    packages=find_packages('python'),
    package_dir={{'':'python'}},
    description='Generated XACC plugins project', 
    long_description='XACC provides a language and hardware agnostic programming framework for hybrid classical-quantum applications.',
    ext_modules=[CMakeExtension('pyxacc{project_shortname_lower}')],
    cmdclass={{'build_ext':CMakeBuild}},# 'install':InstallCommand}},
    zip_safe=False
)

