#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Python Bindings for libeastwood
#
# Copyright (c) 2009 Per Øyvind Karlsen <peroyvind@mandriva.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
import sys, os
from setuptools import setup, Extension
from distutils.ccompiler import get_default_compiler

descr = "Python bindings for libeastwood"
long_descr = """PyEastwood provides a python interface for the libeastwood
library to read and write several data formats used by old Westwood games."""
version = '0.3.1'
version_define = [('VERSION', '"%s"' % version)]

modules = ['eastwood']
c_files = [
        'pycpsfile.cpp',
        'pyeastwood.cpp',
        'pyemcfile.cpp',
        'pyicnfile.cpp',
        'pymapfile.cpp',
        'pypakfile.cpp',
        'pypalfile.cpp',
        'pypalette.cpp',
        'pyshpfile.cpp',
        'pysound.cpp',
        'pystringfile.cpp',
        'pysurface.cpp',
        'pyvocfile.cpp',
        'pywsafile.cpp'
        ]
for i in xrange(len(c_files)):
    c_files[i] = os.path.join("src", c_files[i])

compile_args = []
link_args = []
libraries = ['eastwood']
if get_default_compiler() in ('cygwin', 'emx', 'mingw32', 'unix'):
    compile_args.extend(['-fno-strict-aliasing', '-I../include', '-std=gnu++14'])
    warnflags = ['-Wall', '-Wextra', '-pedantic', '-Weffc++']
    compile_args.extend(warnflags)
    # rpath is really lame, but whatever...
    link_args.extend(['-L../build/src', '-Wl,-rpath=../build/src'])
elif get_default_compiler() in ('msvc'):
    compile_args.append(os.environ['CPPFLAGS'].encode('mbcs'))


extens=[Extension('pyeastwood', c_files, extra_compile_args=compile_args, libraries=libraries, extra_link_args=link_args, define_macros=version_define)]

setup(
    name = 'pyeastwood',
    version = version,
    description = descr,
    author = 'Per Øyvind Karlsen',
    author_email = 'peroyvind@mandriva.org',
    url = 'https://launchpad.net/libeastwood',
    license = 'GPL 3',
    keywords = 'westwood game data format',
    long_description = long_descr,
    platforms = sys.platform,
    classifiers = [
        'Development Status :: 4 - Beta',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License (GPL)',
    ],
    py_modules = modules,
    ext_modules = extens,
    test_suite = 'tests',
    entry_points = {'console_scripts': ['eastwood=eastwood:main']}
)

# vim:ts=4:sw=4:et
