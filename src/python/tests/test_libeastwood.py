#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Python Bindings for libeastwood
#
# Copyright (c) 2009 Per Øyvind Karlsen <peroyvind@mandriva.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public # License as published
# by the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
from pyeastwood import *
import unittest

from hashlib import md5
class TestEmcFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/DUNE.PAK')

    def _test_type(self, filename, typename):
        self.pak.open(filename)

        disassembled = EmcFile(self.pak.read(), EMC_DISASSEMBLE)
        assembled = EmcFile(disassembled.get(), EMC_ASSEMBLE)

        self.assertEqual(disassembled.type, typename)
        self.assertEqual(assembled.type, typename)

    def test_type_BUILD(self):
        self._test_type("BUILD.EMC", "BUILD")

    def test_type_HOUSE(self):
        self._test_type("TEAM.EMC", "HOUSE")

    def test_type_UNIT(self):
        self._test_type("UNIT.EMC", "UNIT")

    def _test_assembly(self, filename):
        self.pak.open(filename)

        fileOrig = self.pak.read()
        sizeOrig = len(fileOrig)
        md5Orig = md5(fileOrig).hexdigest()
        
        disassembled = EmcFile(fileOrig, EMC_DISASSEMBLE)
        assembled = EmcFile(disassembled.get(), EMC_ASSEMBLE)

        fileResult = assembled.get()
        sizeResult = len(fileResult)
        md5Result = md5(fileResult).hexdigest()

        self.assertEqual(sizeOrig, sizeResult)
        self.assertEqual(md5Orig, md5Result)

    def test_assembly_BUILD(self):
        self._test_assembly("BUILD.EMC")

    def test_assembly_HOUSE(self):
        self._test_assembly("TEAM.EMC")

    def test_assembly_UNIT(self):
        self._test_assembly("UNIT.EMC")

class TestMapFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/DUNE.PAK')
        self.pak.open("ICON.MAP")
        self.map = MapFile(self.pak.read())

    def test_size(self):
        self.assertEqual(len(self.map), 27)

    def test_total_size(self):
        totalsize = 0
        for i in xrange(len(self.map)):
            totalsize += len(self.map[i])
        self.assertEqual(totalsize, 716)

    def test_total_value(self):
        totalsize = 0
        for i in xrange(len(self.map)):
            for j in self.map[i]:
                totalsize += j
        self.assertEqual(totalsize, 168192)

class TestPalette(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/DUNE.PAK')
        self.pak.open("IBM.PAL")
        self.file = self.pak.read()
        self.bytearray = bytearray(self.file)
        self.palfile = PalFile(self.file)
        self.palette = self.palfile.getPalette()

    def test_palette(self):
        PAL = self.palette.savePAL()
        self.assertEqual(len(self.file), len(PAL))
        self.assertEqual(md5(self.file).hexdigest(), md5(PAL).hexdigest())
        newPalette = PalFile(PAL).getPalette()
        for i in xrange(len(self.palette)):
            self.assertEqual(self.palette[i], newPalette[i])

def test_main():
    from test import test_support
    test_support.run_unittest(TestEmcFile)
    test_support.run_unittest(TestMapFile)
    test_support.run_unittest(TestPalette)

if __name__ == "__main__":
    test_main()

# vim:ts=4:sw=4:et
