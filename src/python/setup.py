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
import sys
from setuptools import setup, Extension

descr = "Python bindings for libeastwood"
long_descr = """PyEastwood provides a python interface for the libeastwood
library to read and write several data formats used by old Westwood games."""
version = '0.3'
version_define = [('VERSION', '"%s"' % version)]

modules = ['eastwood']
c_files = [
        'src/pycpsfile.cpp',
        'src/pyeastwood.cpp',
        'src/pyemcfile.cpp',
        'src/pyicnfile.cpp',
        'src/pymapfile.cpp',
        'src/pypakfile.cpp',
        'src/pypalfile.cpp',
        'src/pypalette.cpp',
        'src/pyshpfile.cpp',
        'src/pysound.cpp',
        'src/pystringfile.cpp',
        'src/pysurface.cpp',
        'src/pyvocfile.cpp',
        'src/pywsafile.cpp'
        ]

compile_args = ['-fno-strict-aliasing']
warnflags = ['-Wall', '-Wextra', '-pedantic', '-Weffc++', '-Wno-long-long']
compile_args.extend(warnflags)
link_args = ['-leastwood']

extens=[Extension('pyeastwood', c_files, extra_compile_args=compile_args, extra_link_args=link_args, define_macros=version_define)]

setup(
    name = 'pyeastwood',
    version = version,
    description = descr,
    author = 'Per Øyvind Karlsen',
    author_email = 'peroyvind@mandriva.org',
    url = 'https://launchpad.net/libeastwood',
    license = 'GPL 3 ',
    keywords = 'westwood game data format',
    long_description = long_descr,
    platforms = sys.platform,
    classifiers = [
        'Development Status :: 4 - Beta',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU Library or Lesser General Public License (GPL)',
    ],
    py_modules = modules,
    ext_modules = extens,
    test_suite = 'tests',
)

sys.exit(0)

# vim:ts=4:sw=4:et
