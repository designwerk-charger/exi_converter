#!/usr/bin/env python

import io
import os
import sys
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# Import our own version number 
try:
    from version import CURRENT_VERSION
except ModuleNotFoundError as e:
    print("The module version was not found! Please run the set-version.sh first!")
    raise e

with io.open('README.md', encoding='UTF-8') as f:
    long_description = f.read()

NAME = "exi_converter"


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        if 'HOST_DIR' in os.environ:
            tcfcmd=f"-DCMAKE_TOOLCHAIN_FILE={os.environ['HOST_DIR']}/share/buildroot/toolchainfile.cmake"
            print(f'Building most likely within a buildroot environment. So lets use the toolchain option of cmake:{tcfcmd}')
            cmake_args.append(tcfcmd)


        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]
        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]

        build_args += ['--target', 'exi_converter', '--', '-j']


        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''), CURRENT_VERSION)

        print(f"Building {ext.name} with sources {ext.sourcedir} in directory {self.build_temp}")
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

setup(
    name=NAME,
    version=CURRENT_VERSION,
    author="Designwerk Technologies",
    author_email="info@designwerk.com",
    license="Proprietary",
    url="designwerk.com",
    packages=[],
    package_dir={},
    include_package_data=True,
    entry_points={},
    description="MDC CAN Interface Library",
    long_description=long_description,
    long_description_content_type='text/markdown',
    ext_modules=[CMakeExtension('mdccanlib', "")],
    cmdclass=dict(build_ext=CMakeBuild),
    classifiers=[
        "Operating System :: POSIX :: Linux",
        "License :: Other/Proprietary License",
        "Programming Language :: Python :: 3 :: Only"
    ]
)
