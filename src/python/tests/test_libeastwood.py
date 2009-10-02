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

class TestShpFile(unittest.TestCase):
    
    def setUp(self):
        pak = PakFile('DUNE2/DUNE.PAK')
        pak.open("IBM.PAL")
        self.pal = PalFile(pak.read()).getPalette()
        pak.open("UNITS1.SHP")
        self.shp = ShpFile(pak.read(), self.pal)

    def test_surface(self):
        knowngood = (
                (1654, 'ca77e72081cd9fd537c99ae1912cb0be'),
                (1654, 'fa4530a43e65f417d8e967bf0ca27920'),
                (1334, '31adb2e7eb72dfc4aab6db888299a359'),
                (1334, '49b571cf3d6587a9309cf21bd6458867'),
                (1334, 'f377dd13345b1cbebd4e2b441f2a7229'),
                (1334, '21648cf82da0d3d330bb5756f3da533e'),
                (1334, '902bdd604d8ec4392560ca8e1b0cfedf'),
                (1334, 'a30feb3c278927bd5570fe8c9b8261ba'),
                (1654, 'd4cbf0c3c1d112729652d421987ed7d8'),
                (1654, '78a2cdc0ec03f1582c2833afecfc6972'),
                (1654, 'bb6459951e15e89985dfc24cdc7fe698'),
                (1334, 'b412b66922ce0c7519d69cb1ec29fd03'),
                (1334, 'ae005979de6a14388a2bb7b89b484635'),
                (1334, '5daf95b1cb588b320ae09fad271d08aa'),
                (1334, '5e66af5d9781871eeba57127a90aa552'),
                (1334, '8cb12fe18336814309ed5a7166d29ca5'),
                (1334, 'bf834cb44f9dbceeb2d53552e8d7e53a'),
                (1334, 'ed40fe14fbfc4e2765e23f90915c4a9e'),
                (1334, '5e670a9430d2bb15bf8a791ab445e178'),
                (1334, '45a0da1d57dc22493350c9b8a8e91cbc'),
                (1334, '347296bebd13d32d5151fe9463c4d336'),
                (1334, '652cc7613d68ee82fd1fb7601e49a935'),
                (1334, '1b8cad7c7628a3d7cd6b8916619b6cd3'),
                (1090, 'ce63de1c6345f6e613dae3f4e26d04c6'),
                (1094, 'c14496d085e4d3a8a5a92ac8dff558f8'),
                (1142, '9dd72349d1408c81be6c7f1f82934cc7'),
                (1142, 'e8f350a293786ac3095dcbbc1a18c432'),
                (1142, '88cfa1a66946bfda25ce5f5654dc54ec'),
                (1142, '2fd739f391ba818b5318fc82f78ab1a9'),
                (1462, 'b313b10c60b754056e448cd4be718157'),
                (1462, '3455f75e65bb8c06ea3dd8831642e162'),
                (1462, '7fac272626f165c62cc03dfdd0ea70b9'),
                (1334, 'fe5651302fee1fc61859ee7ed45160c0'),
                (1334, 'd4510a73612fccc02e38cdd9d1accc81'),
                (1334, '4aadbd8cc409544d7d9c2cc0c65fc1cf'),
                (1334, '52aa2bbb5e3672e158bb4bed5b05085e'),
                (1334, '2c4f3720b15763e94bb796aeed153c15'),
                (1654, 'c07753a8501d0cd2ddea3f0aaa9d50ee'),
                (1654, '5fae955edd93bcfa4a0be192dcaf0efb'),
                (1654, 'd0dd56d225bed0c0ff6b50025960f707'),
                (1654, '29979c6c2f9e00a736c8548615a09f38'),
                (1654, '07baa5bc1b21433a6dbe6e40a81a9090'),
                (1654, '719b19da7ed78e1eecdf8049beb4ee90'),
                (1654, '12797591bbc387062400d4f6e226f2ee'),
                (1654, 'b49ab8c2e5063ba72ddf048297e82d9f'),
                (1654, 'caad58e68b5c8d514c385ebc2d34fdb7'),
                (1654, '796e490de64a3271615831d5fad36405'),
                (1654, '1808a8a64e0d3a066533813d5989dec0'),
                (1654, 'cdc2a6a94bef4cadf9455c7184e4af7e'),
                (1654, 'b30bcab6b95bd5a32248ab0c2da76df1'),
                (1654, 'a9426a79508515684038792f26c46f7a'),
                (1654, 'ed9f7936829cf8162d4bc8f53e2d49fe'),
                (1654, '1af4bc2dba7490dd636bba8499c26680'),
                (1654, '63f364cb040ddf38718f525d0b4670ec'),
                (1654, '0a92ea475baff5fce99eb7cec7046bf3'),
                (1654, '0615a6f2adf34cafdcd55dfb615d4f62'),
                (1654, '017f94b2b26f046ac2d1de986a509768'),
                (1654, '357410d71f0bdb68d28528168494f430'),
                (1654, '374df99effdd08943c572604f736290b'),
                (1654, '469abc693b3cbfd820fcd8834fa8ffca'),
                (1654, '2a299c5f424828725c85109532731d9c'),
                (1654, '644fb6aa6d7fe25182592f2e985f1111'),
                (1082, '931256af49ce1402572c4947ed8a2df9'),
                (1082, '931256af49ce1402572c4947ed8a2df9'),
                (1082, '931256af49ce1402572c4947ed8a2df9'),
                (1082, '931256af49ce1402572c4947ed8a2df9'),
                (1082, '931256af49ce1402572c4947ed8a2df9'),
                (1654, '929d528b6210c7f0f4a1f4ac2a084e88'),
                (1654, 'e8c14ffe21e8877cec499faca03f179b'),
                (1654, 'dfba5c4ce90f8a9a81f4a8b359ad7ec3'),
                (1654, '9ccb86a2c00e207842a144a26061941b'),
                (1654, '311aab977c53c33d1f7e0a4b522a794a'),
                (1334, '9e5b09d70f52ad30dc3a854bba58c85a'),
                (1334, 'e63be58486a237f5696a2a2ebd1c809c'),
                (1334, '47d702878319fee28a467b7fab0790fc'),
                (1334, '14d4c5ab47573ad35be47d7e059b911c'),
                (1334, 'fa8c7f768764b0320e0488bbb29fd868'),
                (1334, '56be823d2e4dd7e9e437537d57e45a2a'),
                (1334, 'e8b50ed900b8c73649548ad4f9fefd21'),
                (1334, 'b7359a44f2fc0d748290044aaf4587dc'),
                (1334, 'aa575a9e27c881342797caccff2d07b6'),
                (1334, '6443fd916929f893846a24f5edd02981'),
                (1334, 'a52a129dbe25a43858d1f6f999daef4d'),
                (1334, 'dbdb3b7361f96cc8576b85292c363c7c'),
                (1334, '373ea9e99fd5011cd4c2c39de80e6444'),
                (1334, '654bb6f502a66c5bb4ee0e668bdf2f17'),
                (1334, '561a6b741db474dd1450ba912cde367f')
                )
        for i in xrange(self.shp.size):
            surface = self.shp.getSurface(i)
            # We test with saveBMP() to have saveBMP() tested as well 
            self.assertEqual(len(surface.saveBMP()), knowngood[i][0])
            self.assertEqual(md5(surface.saveBMP()).hexdigest(), knowngood[i][1])

    def test_array(self):
        surface = self.shp.getSurfaceArray(3, 1, (29 | TILE_NORMAL, 30 | TILE_NORMAL, 31 | TILE_NORMAL))
        bmp = surface.saveBMP()
        self.assertEqual(len(bmp), 2230)
        self.assertEqual(md5(surface.saveBMP()).hexdigest(), '94d854d02b3dfa72c8dcd3d7d5f5f980')


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
    test_support.run_unittest(TestShpFile)
    test_support.run_unittest(TestStringFile)

if __name__ == "__main__":
    test_main()

# vim:ts=4:sw=4:et
