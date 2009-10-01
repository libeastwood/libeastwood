#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Python Bindings for libeastwood
#
# Copyright (c) 2009 Per Øyvind Karlsen <peroyvind@mandriva.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
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
from glob import glob
import unittest

from hashlib import md5
class TestPakFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/MERC.PAK')

    def test_listfiles(self):
        files = ('MATRE.VOC', 'MHARK.VOC', 'MORDOS.VOC')
        self.assertEqual(self.pak.listfiles(), files)

    def test_seek(self):
        for f in glob('DUNE2/*.PAK'):
            pak = PakFile(f)
            for pf in pak.listfiles():
                pak.open(pf)
                first = pak.read()
                pak.seek(0)
                last = pak.read()
                self.assertEqual(len(first), len(last))
                self.assertEqual(md5(first).hexdigest(), md5(last).hexdigest())
                pak.close()

    def test_md5(self):
        knowngood = {
                'MATRE.VOC' : (15394, 'fe02273c381ca5582497be0dab89ebd5'),
                'MHARK.VOC' : (13858, '089d3bc65f10a26b322dd318ad390dd3'),
                'MORDOS.VOC' : (16290, '59869ea8e94ed46fe4202aaff761ad55')
                }
        for f in self.pak.listfiles():
            self.pak.open(f)
            fileData = self.pak.read()
            self.assertEqual(len(fileData), knowngood[f][0])
            self.assertEqual(md5(fileData).hexdigest(), knowngood[f][1])

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

class TestIcnFile(unittest.TestCase):
    
    def setUp(self):
        pak = PakFile('DUNE2/DUNE.PAK')
        pak.open("IBM.PAL")
        pal = PalFile(pak.read()).getPalette()
        pak.open("ICON.MAP")
        self.map = MapFile(pak.read())
        pak.open("ICON.ICN")
        self.icn = IcnFile(pak.read(), self.map, pal)

    def test_size(self):
        self.assertEqual(self.icn.size, 389)

    def test_tiles_md5(self):
        md5sums = (
                '24bf562fef4103f740c42472de1af073',
                '1fd32705ecbccb85f66885dd40260a3e',
                '45a9b8159c3e842cd48c8bbc78dc2903',
                'e56d9a2abb690f734fe7e7c5f16530e8',
                '1ccd896084894d6098697d8f35d31205',
                'bd47ff2c3d4b5be36c6e3e65d83a4f08',
                'af238c100a819d02d843d9277bef6325',
                'ac99dd6fbf23a97c8c1e12674842e790',
                '7626f228b242bc27c2f808e578bb75cd',
                'cad2a3680cf7ab906015987358d829e4',
                '619138ed6573f13894d6e32a60b6e886',
                '68168c408b1e542716eb44aa8561af29',
                '05be3de8775004d2362d2907083fd825',
                '7648f773fa9d2e3845bba56e14bbb55e',
                '3378d42b25b1d9e007f621456b8f34ac',
                '46920d9cde6d8c9e07b716d28e25af36',
                '838d72c56ea4ed6b0b9a0a263757757e',
                '5123d09b316fe00f17e16290a53ff69f',
                '84fa73b8e99fce49a0759b2b5cce64fc',
                '7a725033ab1a92ae3d089c4efc4ed3c3',
                '7e4d4ccb46056bea8c4465db419e884d',
                '270e7185cd1b2a1e3db3f00004cb40cc',
                '4513370b5d5a86136e4702c2630c0c0d',
                '2be06d3960c94074b70f776f31ab301b',
                'eca3164094463e7db4e820ad480891f8',
                '2af9d386789727bb0f2e6933c4b2d4e5',
                '4421d8fa47fcd1812b6bef532604d917'
                )

        for i in xrange(len(self.map)):
            surface = self.icn.getTiles(i, False)
            self.assertEqual(md5(surface.getPixels()).hexdigest(), md5sums[i])

    def test_tiles_frameByFrame_md5(self):
        md5sums = (
                '24bf562fef4103f740c42472de1af073',
                'c37958d522c0db8d40097c5e0ac384ea',
                '3cb149195a412e635c6c71b89c73dcd0',
                '64cf64b707491d822148a7e5b210783b',
                '5f4922f04b71a1349a1a656c7a33a05f',
                'f15c98c6b50645c554e6b4cf04973291',
                '6a6741826ccfff44163e39866c244cb1',
                '55ab13f3bf270036859407d1d3ed224a',
                '7626f228b242bc27c2f808e578bb75cd',
                'e89e239cd1bae942c57f1b4ec01476a1',
                '619138ed6573f13894d6e32a60b6e886',
                '6b82e20160716d8396daaeb885f1d0e0',
                'f966e1b397bfa01a1d598916a864e399',
                'cc9e172938c1a16291af16439234bec5',
                '41ccbb3ce52e7396634f8388c75e36a0',
                '48e35ea6e41bb6ee50bb3c216181cd09',
                '3317637de65b2348fd5c6fd21dc46629',
                '1eb62d289dcab65c811d3734a4459ed6',
                '7d6a12a196b71f38d796a8240450530a',
                '1a0b953b6df77a909b8ac795b2d01017',
                '32e0e3ab9a73819faabbd80244d2fbb8',
                '35905b91c256292e7efff3527d52b26b',
                '203c3ce5fa4818666612f82b7a0516b6',
                '2be06d3960c94074b70f776f31ab301b',
                'eca3164094463e7db4e820ad480891f8',
                '8c7338dea7bd1d3f459d30ff695304eb',
                '66c62f25456d518682ac76a279f36515'
                )
        for i in xrange(len(self.map)):
            surface = self.icn.getTiles(i, True)
            self.assertEqual(md5(surface.getPixels()).hexdigest(), md5sums[i])


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

class TestStringFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/ENGLISH.PAK')

    def test_uncompressed(self):
        self.pak.open("CREDITS.ENG")
        stringfile = StringFile(self.pak.read())
        self.assertEqual(stringfile.size, 98)
        self.assertEqual(stringfile.getString(0), "DUNE II")
        self.assertEqual(stringfile.getString(stringfile.size-3), "DUNE II")

    def test_mission(self):
        self.pak.open("TEXTA.ENG")
        stringfile = StringFile(self.pak.read())
        self.assertEqual(stringfile.size, 39)
        self.assertEqual(stringfile.getString(1), "win text.")
        self.assertEqual(stringfile.getString(stringfile.size-1),\
                "Your defects must have been inherited. Nobody could possible learn the colossal stupidity that you have displayed.")

def test_main():
    from test import test_support
    test_support.run_unittest(TestPakFile)
    test_support.run_unittest(TestEmcFile)
    test_support.run_unittest(TestMapFile)
    test_support.run_unittest(TestIcnFile)
    test_support.run_unittest(TestPalette)
    test_support.run_unittest(TestStringFile)

if __name__ == "__main__":
    test_main()

# vim:ts=4:sw=4:et
