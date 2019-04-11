from setuptools import setup, extension
from setuptools.command.build_ext import build_ext
import sys
import os
import platform

cwd = os.path.abspath(os.path.dirname(__file__))
if not os.path.exists(os.path.join(cwd, 'PKG-INFO')):
    try:
        from Cython.Build import cythonize
        USE_CYTHON = True
    except ImportError:
        USE_CYTHON = False
else:
    USE_CYTHON = False

_PY2 = sys.version_info.major == 2

# Change directories so this works when called from other locations. Useful in build systems that build from source.
setup_folder_loc = os.path.dirname(os.path.abspath(__file__))
os.chdir(setup_folder_loc)

# Add __version__, __author__, __authoremail__, __description__ to this namespace
path_to_package_info = os.path.join('.', 'package_info.py')
if _PY2:
    execfile(path_to_package_info)
else:
    exec(open(path_to_package_info).read())

extra_compile_args = {
    'msvc': ['/std:c++latest', '/MT', '/EHsc'],
    'unix': ['-std=c++11', '-Wextra', '-Wno-format-security', '-Ofast', '-fomit-frame-pointer', '-DNDEBUG', '-fno-rtti'],
}

extra_link_args = {
    'msvc': [],
    'unix': ['-std=c++11'],
}

if '--debug' in sys.argv or '-g' in sys.argv or 'CPPDEBUG' in os.environ:
    extra_compile_args['msvc'].append('/DCPPDEBUG')
    extra_compile_args['unix'] = ['-std=c++1y', '-w',
                                  '-O0', '-g', '-fipa-pure-const', '-DCPPDEBUG']


class build_ext_compiler_check(build_ext):
    def build_extensions(self):
        compiler = self.compiler.compiler_type

        compile_args = extra_compile_args[compiler]
        for ext in self.extensions:
            ext.extra_compile_args = compile_args

        link_args = extra_compile_args[compiler]
        for ext in self.extensions:
            ext.extra_compile_args = link_args

        build_ext.build_extensions(self)


class Extension(extension.Extension, object):
    pass


ext = '.pyx' if USE_CYTHON else '.cpp'

extensions = [Extension(
    name="minorminer",
    sources=["./python/minorminer" + ext],
    include_dirs=['', './include/'],
    language='c++',
)]

if USE_CYTHON:
    extensions = cythonize(extensions)

os.environ["MACOSX_DEPLOYMENT_TARGET"] = platform.mac_ver()[0]

setup(
    name="minorminer",
    description=__description__,
    long_description="minorminer is a tool for finding graph minors, developed to embed Ising problems onto quantum annealers (QA). Where it can be used to find minors in arbitrary graphs, it is particularly geared towards the state of the art in QA: problem graphs of a few to a few hundred variables, and hardware graphs of a few thousand qubits.",
    author=__author__,
    author_email=__authoremail__,
    url="https://github.com/dwavesystems/minorminer",
    version=__version__,
    license="Apache 2.0",
    ext_modules=extensions,
    cmdclass={'build_ext': build_ext_compiler_check}
)
