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

openssl_root_dir = None

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
        install_prefix = script_path + '/' + self.build_lib 
        print(dir(self))
        print(self.build_lib)
  
        cmake_args = ['-DPYTHON_INCLUDE_DIR=' + sysconfig.get_paths()['platinclude'], #sys.executable,
                      '-DCMAKE_INSTALL_PREFIX='+install_prefix,
		      '-DFROM_SETUP_PY=TRUE']
        
        if not openssl_root_dir == None:
            cmake_args.append('-DOPENSSL_ROOT_DIR='+openssl_root_dir)

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

class InstallCommand(InstallCommandBase):
    user_options = InstallCommandBase.user_options + [
		    ('openssl-dir=', None, 'Specify OPENSSL_ROOT_DIR')
		    ]

    def initialize_options(self):
        InstallCommandBase.initialize_options(self)
        self.openssl_dir = '/usr/local/opt/openssl' if platform.system() == 'Darwin' else '/usr/lib64'

    def finalize_options(self):
        global openssl_root_dir
        openssl_root_dir = self.openssl_dir
        InstallCommandBase.finalize_options(self)

    def run(self):
        InstallCommandBase.run(self)

s = setup(
    name='xacc',
    version='0.1.0',
    author='Alex McCaskey',
    author_email='xacc-dev@eclipse.org',
    packages=find_packages('python'),
    package_dir={'':'python'},
    package_data={'':[env['HOME']+'/.xacc/lib/python/pyxacc.so']},
    description='Hardware-agnostic quantum programming framework',
    long_description='XACC provides a language and hardware agnostic programming framework for hybrid classical-quantum applications.',
    ext_modules=[CMakeExtension('pyxacc')],
    cmdclass={'build_ext':CMakeBuild},# 'install':InstallCommand},
    scripts=['tools/framework/xacc-framework'],
    zip_safe=False
)

#try:
#   import pyxacc as xacc
#   xaccLocation = os.path.dirname(os.path.realpath(xacc.__file__))
#
#except:
#   pass
