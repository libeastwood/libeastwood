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
from shutil import copy
from os import unlink
from os.path import getsize
import unittest
from hashlib import md5

class TestPakFile(unittest.TestCase):
    
    def setUp(self):
        self.filename = ('test-MERC.PAK')
        copy('DUNE2/MERC.PAK', self.filename)
        self.pak = PakFile(self.filename)

        self.filename2 = ('test-INTROVOC.PAK')
        copy('DUNE2/INTROVOC.PAK', self.filename2)
        self.pak2 = PakFile(self.filename2)

    def test_listfiles(self):
        files = ('MATRE.VOC', 'MHARK.VOC', 'MORDOS.VOC')
        self.assertEqual(self.pak.listfiles(), files)

    def test_seek(self):
        for f in glob('DUNE2/*.PAK'):
            pak = PakFile(f)
            for pf in pak.listfiles():
                pak.open(pf)
                first = pak.read()
                pak.seekp(0)
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

    def test_delete(self):
        knowngood = {}
        filelist = list(self.pak.listfiles())
        for f in filelist:
            self.pak.open(f)
            data = self.pak.read()
            knowngood[f] = (len(data), md5(data).hexdigest())
        
        for f in self.pak.listfiles():
            self.assertEqual(filelist, list(self.pak.listfiles()))
            self.pak.open(f)
            data = self.pak.read()
            self.assertEqual(len(data), knowngood[f][0])
            self.assertEqual(md5(data).hexdigest(), knowngood[f][1])
            self.pak.delete(f)
            filelist.remove(f)
        del self.pak
        self.assertEqual(getsize(self.filename), 0)

    def test_delete_reverse(self):
        knowngood = {}
        filelist = list(self.pak2.listfiles())
        for f in filelist:
            self.pak2.open(f)
            data = self.pak2.read()
            knowngood[f] = (len(data), md5(data).hexdigest())
        
        reverse_filelist = list(self.pak2.listfiles())
        reverse_filelist.reverse()
        for f in reverse_filelist:
            self.assertEqual(filelist, list(self.pak2.listfiles()))
            self.pak2.open(f)
            data = self.pak2.read()
            self.assertEqual(len(data), knowngood[f][0])
            self.assertEqual(md5(data).hexdigest(), knowngood[f][1])
            self.pak2.delete(f)
            filelist.remove(f)
        del self.pak2
        self.assertEqual(getsize(self.filename2), 0)

    def test_write_append(self):
        knowngood = {}
        filelist = list(self.pak2.listfiles())
        for f in filelist:
            self.pak2.open(f)
            data = self.pak2.read()
            knowngood[f] = [len(data), md5(data).hexdigest()]
        size = getsize(self.filename2)

        buf = "123456789"
        for i in xrange(0, len(filelist), 2):
            self.pak2.open(filelist[i], "a+")
            self.pak2.write(buf)
            bufmd5 = md5(self.pak2.read()).hexdigest()
            self.pak2.close()
            size += len(buf)
            knowngood[filelist[i]][0] += len(buf)
            knowngood[filelist[i]][1] = bufmd5

        for f in filelist:
            self.assertEqual(filelist, list(self.pak2.listfiles()))
            self.pak2.open(f)
            data = self.pak2.read()
            self.assertEqual(len(data), knowngood[f][0])
            self.assertEqual(md5(data).hexdigest(), knowngood[f][1])

        del self.pak2
        self.assertEqual(getsize(self.filename2), size)

    def test_write_truncate(self):
        knowngood = {}
        filelist = list(self.pak2.listfiles())
        for f in filelist:
            self.pak2.open(f)
            data = self.pak2.read()
            knowngood[f] = [len(data), md5(data).hexdigest()]
        size = getsize(self.filename2)

        buf = "123456789"
        bufmd5 = md5(buf).hexdigest()
        for i in xrange(0, len(filelist), 2):
            self.pak2.open(filelist[i], "r")
            size -= len(self.pak2.read())
            self.pak2.close()
            self.pak2.open(filelist[i], "w")
            self.pak2.write(buf)
            self.pak2.close()
            size += len(buf)
            knowngood[filelist[i]][0] = len(buf)
            knowngood[filelist[i]][1] = bufmd5

        for f in filelist:
            self.assertEqual(filelist, list(self.pak2.listfiles()))
            self.pak2.open(f)
            data = self.pak2.read()
            self.assertEqual(len(data), knowngood[f][0])
            self.assertEqual(md5(data).hexdigest(), knowngood[f][1])

        del self.pak2
        self.assertEqual(getsize(self.filename2), size)

    def test_create_delete(self):
        knowngood = {}
        filelist = list(self.pak.listfiles())
        filesize = getsize(self.filename)
        for f in filelist:
            self.pak.open(f)
            data = self.pak.read()
            knowngood[f] = (len(data), md5(data).hexdigest())
        
        self.assertEqual(list(self.pak.listfiles()), filelist)

        f = filelist[0]
        self.pak.open(f)
        data = self.pak.read()
        self.assertEqual(len(data), knowngood[f][0])
        self.assertEqual(md5(data).hexdigest(), knowngood[f][1])
        # entry in index is filename + 4 (sizeof(uint32_t)) + 1 (0 terminator)
        filesize -= (len(filelist[0]) + 4 + 1)
        filesize -= len(data)
        self.pak.delete(f)
        filelist.remove(f)
        
        buf = "123456789"
        newname = "TEST.TXT"
        self.pak.open(newname, "w")
        self.pak.write(buf)
        self.pak.close()
        filesize += len(buf)
        filelist.append(newname)
        filesize += len(newname) + 4 + 1

        self.assertEqual(list(self.pak.listfiles()), filelist)

        self.pak.open(newname)
        data = self.pak.read()
        self.assertEqual(len(data), len(buf))
        self.assertEqual(data, buf)
        self.pak.close()

        del self.pak
        self.assertEqual(getsize(self.filename), filesize)

    def test_create_new(self): #FIXME:
        pak = PakFile("test-TEST.PAK", True)
        pak.open("SOMEFILE.TXT", "w")
        pak.write("123456789")
        pak.close()
        pak.open("FILETWO.TXT", "w")
        pak.write("abcdefghijklmnopqrstuvwxyz")
        pak.close()

    def tearDown(self):
        unlink("test-MERC.PAK")
        unlink("test-INTROVOC.PAK")

class TestCpsFile(unittest.TestCase):
    
    def test_with_palette(self):
        pak = PakFile('DUNE2/DUNE.PAK')
        pak.open('IBM.PAL')
        pal = PalFile(pak.read()).getPalette()
        pak.open('SCREEN.CPS')
        surface = CpsFile(pak.read(), pal).getSurface()
        data = surface.getPixels()
        self.assertEqual(len(data), 64000)
        self.assertEqual(md5(data).hexdigest(), '72934c9772cfe17b76dd4df8a829cfc1')

    def test_without_palette(self):
        pak = PakFile('DUNE2/INTRO.PAK')
        pak.open('VIRGIN.CPS')
        surface = CpsFile(pak.read()).getSurface()
        data = surface.getPixels()
        self.assertEqual(len(data), 64000)
        self.assertEqual(md5(data).hexdigest(), 'f511b8d1c2ab3da170f483609174b489')


class TestEmcFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/DUNE.PAK')

    def _test_type(self, filename, typename):
        self.pak.open(filename)

        disassembled = EmcFile(self.pak.read(), 'd')
        assembled = EmcFile(disassembled.get(), 'a')

        self.assertEqual(disassembled.type, typename)
        self.assertEqual(assembled.type, typename)

    def test_type_BUILD(self):
        self._test_type("BUILD.EMC", "BUILD")

    def test_type_TEAM(self):
        self._test_type("TEAM.EMC", "TEAM")

    def test_type_UNIT(self):
        self._test_type("UNIT.EMC", "UNIT")

    def _test_assembly(self, filename):
        self.pak.open(filename)

        fileOrig = self.pak.read()
        sizeOrig = len(fileOrig)
        md5Orig = md5(fileOrig).hexdigest()
        
        disassembled = EmcFile(fileOrig, 'd')
        assembled = EmcFile(disassembled.get(), 'a')

        fileResult = assembled.get()
        sizeResult = len(fileResult)
        md5Result = md5(fileResult).hexdigest()

        self.assertEqual(sizeOrig, sizeResult)
        self.assertEqual(md5Orig, md5Result)

    def test_assembly_BUILD(self):
        self._test_assembly("BUILD.EMC")

    def test_assembly_TEAM(self):
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
            bmp = surface.saveBMP()
            self.assertEqual(len(bmp), knowngood[i][0])
            self.assertEqual(md5(bmp).hexdigest(), knowngood[i][1])

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

class TestVocFile(unittest.TestCase):
    
    def setUp(self):
        self.pak = PakFile('DUNE2/VOC.PAK')

    def test_voc(self):
        knowngood = {
                'VSCREAM1.VOC' : (5861, '272b0556fc0b964d5362ddc7b36c2c91'),
                'VSCREAM2.VOC' : (5081, '542c3d64eb3a1a816c99caf82a350bcb'),
                'VSCREAM3.VOC' : (5876, '0a88c1d209a93c0201a0e3de5c787446'),
                'VSCREAM4.VOC' : (4653, '1a3954e042f3b71e90a741146dee49ba'),
                'VSCREAM5.VOC' : (5613, '16acbd15c69586cde1a6f6de247b9799'),
                'ZAFFIRM.VOC' : (10521, '8de5cdd6536d87f203415ffeb4dcea1f'),
                'AFFIRM.VOC' : (10521, '8de5cdd6536d87f203415ffeb4dcea1f'),
                'BUTTON.VOC' : (1133, '6ce440604a1b8cabfba5f97cfe1d4a37'),
                'CRUMBLE.VOC' : (5421, '22ef34db9ebab94ff9cbce8453d1c82d'),
                'DROPEQ2P.VOC' : (4963, '828f62c6cdac1bd62ecbcbf212c3b877'),
                'EXCANNON.VOC' : (3181, 'd7dc5a10e33d6d8457dbc32863e5b5c4'),
                'EXDUD.VOC' : (3973, '35e0b9320b1bb94587293b3b66972113'),
                'EXGAS.VOC' : (5037, '0cf69559c1ac6e7107c2cd8bec4c195f'),
                'EXLARGE.VOC' : (17093, 'c4fdc4d6881fbb43e46d726eb6cfe804'),
                'EXMED.VOC' : (7213, '879f55c2c3f8f9b4bcd462af4d05de08'),
                'EXSAND.VOC' : (5681, 'e1cb1fbd665925f20d5996b220a10710'),
                'EXSMALL.VOC' : (3181, 'd7dc5a10e33d6d8457dbc32863e5b5c4'),
                'EXTINY.VOC' : (8463, '867d1f291d787854b47d6d6519b9b95b'),
                'GUN.VOC' : (3321, '7e466f7655603ae19ed0a41dbd5dc87c'),
                'GUNMULTI.VOC' : (7345, '19a2e257b84c9f51635d7de120afacab'),
                'MISLTINP.VOC' : (10021, 'b524cbda8285a696b5aecc83ec954c78'),
                'ZMOVEOUT.VOC' : (10401, 'f2a1c2739624fd4ff662a2f73a62050e'),
                'MOVEOUT.VOC' : (10401, 'f2a1c2739624fd4ff662a2f73a62050e'),
                'ZOVEROUT.VOC' : (15377, 'd5f57024879a9307138f1284e047476a'),
                'OVEROUT.VOC' : (15377, 'd5f57024879a9307138f1284e047476a'),
                'POPPA.VOC' : (15425, '507789cfc05720e97e82859b1c8914d9'),
                'ZREPORT1.VOC' : (9811, '5e1b72b81e3d78b761507f3e54d31b00'),
                'REPORT1.VOC' : (9811, '5e1b72b81e3d78b761507f3e54d31b00'),
                'ZREPORT2.VOC' : (11260, '6af19f722e740f4d37949e0d7371e9f6'),
                'REPORT2.VOC' : (11260, '6af19f722e740f4d37949e0d7371e9f6'),
                'ZREPORT3.VOC' : (12143, 'ce7b729ca44a0fe14310639a5bf65336'),
                'REPORT3.VOC' : (12143, 'ce7b729ca44a0fe14310639a5bf65336'),
                'ROCKET.VOC' : (8786, 'ef486074183745fa13d66ca23d4e2677'),
                'SANDBUG.VOC' : (17446, '41eb275b91be7620edc45f6de0f29811'),
                'SQUISH2.VOC' : (8258, '063ffe5c1afd15397d7cdf2b74eca2d5'),
                'STATICP.VOC' : (19695, 'fefdcd82f68a37ea6925b827f1e37faf'),
                'WORMET3P.VOC' : (10353, 'c7b00abd0af22247b436c65845715223')
                }

        for f in self.pak.listfiles():
            self.pak.open(f)
            voc = VocFile(self.pak.read())
            sound = voc.getSound()
            wav = sound.saveWAV()
            self.assertEqual(len(wav), knowngood[f][0])
            self.assertEqual(md5(wav).hexdigest(), knowngood[f][1])

class TestSound(unittest.TestCase):
    
    def setUp(self):
        pak = PakFile('DUNE2/INTROVOC.PAK')
        pak.open("DUNE.VOC")
        self.sound = VocFile(pak.read()).getSound()

    def test_resampled_interpolator(self):
        md5sum = (
                '1218e827984c486f3dbf7b22dfe74f37',
                '9ac8a1918a99f75fd3c7b8a03eaaf616',
                '27a2ee5d7e738ab1ea420ec3992a7219',
                'df7033f4be5eca2a53d29808cd13927a',
                '08191f84cc8b21a877556a2b71dbb522'
                )
        for i in xrange(I_SINC_BEST_QUALITY, I_LINEAR+1):
            resampled = self.sound.getResampled(2, 44100, FMT_S16LE, i)
            self.assertEqual(md5(resampled.getBuffer()).hexdigest(), md5sum[i])

    def test_resampled_format(self):
        knowngood = {
                FMT_S16BE : (184224, '6a23cfd17e0462dbd87f597efce71118'),
                FMT_S16LE : (184224, '08191f84cc8b21a877556a2b71dbb522'),
                FMT_S8 : (92112, '55b0b1e8f02a421ccf1bae0c85f5830d'),
                FMT_U16BE : (184224, 'b4b1c2ece22f527274b68c0665d54faf'),
                FMT_U16LE : (184224, 'ba6346590359a3ef1a750b36dc835e86'),
                FMT_U8 : (92112, 'bcf7e3a28632e6b6b8225650781ce497')
                }

        for f in knowngood.keys():
            resampled = self.sound.getResampled(2, 44100, f, I_LINEAR)
            buffer = resampled.getBuffer()
            self.assertEqual(len(buffer), knowngood[f][0])
            self.assertEqual(md5(buffer).hexdigest(), knowngood[f][1])

    def test_resampled_rate(self):
        knowngood = {
                11025 : (46060, '93d3122c776254644ff0e22163471436'),
                22050 : (92112, '77ad224e856884d8b39580b0bce3a230'),
                44100 : (184224, '08191f84cc8b21a877556a2b71dbb522'),
                48000 : (200516, '715bf0c71f026c487ed359652d30395c'),
                96000 : (401028, '210113c876e4ff9d84425bff7c9db280'),
                }

        for r in knowngood.keys():
            resampled = self.sound.getResampled(2, r, FMT_S16LE, I_LINEAR)
            buffer = resampled.getBuffer()
            self.assertEqual(len(buffer), knowngood[r][0])
            self.assertEqual(md5(buffer).hexdigest(), knowngood[r][1])

    def test_resampled_channels(self):
        knowngood = {
                1 : (92112, 'b18d59a2d448ad6af978eb225fc1ca19'),
                2 : (184224, '08191f84cc8b21a877556a2b71dbb522')
                }

        for c in knowngood.keys():
            resampled = self.sound.getResampled(c, 44100, FMT_S16LE, I_LINEAR)
            buffer = resampled.getBuffer()
            self.assertEqual(len(buffer), knowngood[c][0])
            self.assertEqual(md5(buffer).hexdigest(), knowngood[c][1])

class TestSurface(unittest.TestCase):
    
    def setUp(self):
        pak = PakFile('DUNE2/DUNE.PAK')
        pak.open('BENE.PAL')
        pal = PalFile(pak.read()).getPalette()
        pak.open('MENTATM.CPS')
        self.surface = CpsFile(pak.read(), pal).getSurface()

    def test_scalers(self):
        knowngood = {
                Scale2X : (640, 400, 256000, 'a6d36012e54c37d2b0d96e9a49588a2e'),
                Scale2X3 : (640, 600, 384000, '80370eb0a6bd47e1eca9e49433e793e6'),
                Scale2X4 : (640, 800, 512000, '4c5905cf95b3846f62729f4be1b86dcb'),
                #fixme: Scale3X : (960, 600, 576000, '8af59019053330c70e7f15da84f26e78'),
                Scale4X : (1280, 800, 1024000, 'c8945be671a5f10987850f5f1e3b87df')
                }

        for s in knowngood.keys():
            surface = self.surface.getScaled(s)
            pixels = surface.getPixels()
            self.assertEqual(surface.width, knowngood[s][0])
            self.assertEqual(surface.height, knowngood[s][1])
            self.assertEqual(len(pixels), knowngood[s][2])
            self.assertEqual(md5(pixels).hexdigest(), knowngood[s][3])

class TestWsaFile(unittest.TestCase):
    
    def setUp(self):
        pak = PakFile('DUNE2/DUNE.PAK')
        pak.open("IBM.PAL")
        self.pal = PalFile(pak.read()).getPalette()
        self.pak = PakFile('DUNE2/INTRO.PAK')
        self.pak.open("INTRO8A.WSA")
        self.wsa = WsaFile(self.pak.read(), self.pal)
        self.pak.close()

    def test_regular(self):
        knowngood = (
                '9148651e7f2e8b794ab796a053c6cf88',
                'd54c1a08f0e009b280102c9ed36bfe98',
                '9ba1ff38a3c61bbf75992aa501fe340e',
                'd65eaa080de4bc876f894d075b0c387d',
                'd032da79612bcf24624cb43527fcfbdc',
                '6c88e31eada690c894cf591a1a7622b7',
                'bb07b793fc540aa88f999c3458807bd9',
                'dfc9ed7aec16102ffd310b0fe2ab4c36',
                'a7a96ea4fbdbc56efb316305c43d3aed',
                '5c71c825b0b63959704bbee655e0207b',
                '9cf259ada755e3a895f0921f62bf0104',
                '91207d05f95a83e8d51013073b6f9741',
                '26eef82435b6d153fbbc3178c854ac97',
                '52f2c5d80aaf14274bb6de8519b5c6f0',
                'e428f385d3569494050b214fe71f861a',
                'efa7cc26e0fbb49f2961a880ce7efe63'
                )

        for i in xrange(self.wsa.size):
            surface = self.wsa.getSurface(i)
            self.assertEqual(knowngood[i], md5(surface.getPixels()).hexdigest())

    def test_continuation(self):
        knowngood = (
                'efa7cc26e0fbb49f2961a880ce7efe63',
                '94409132c59b6de44f481472b69905e1',
                '04e59ba5a71e84463b02bff1223780ac',
                '3d56cff2383305dfc71ee64895c467f7',
                'b16318250d082bb6ecc495cb2e7dd27b',
                '7343fb4b9c4f95c0ab3d6b4139e37ce5',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                'b0c90452f5e09bbd7073d1c4d52a18b3',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '98ed228bca34c12324ad1906e9bf9cce',
                'ee902ee7e9936208fea91b673d1264d3',
                '69e1d88083b5a669e2859dccad1060b2',
                '8887132f083c424b8d8bcf1096b4b961',
                'decaf4a3d73ff970fc8dfb69cb13a1fb'
                )

        firstFrame = self.wsa.getSurface(self.wsa.size-1)
        self.pak.open("INTRO8B.WSA")
        wsa = WsaFile(self.pak.read(), self.pal, firstFrame)

        for i in xrange(wsa.size):
            surface = wsa.getSurface(i)
            self.assertEqual(knowngood[i], md5(surface.getPixels()).hexdigest())

        knowngood = (
                '8887132f083c424b8d8bcf1096b4b961',
                '756c1e380466a477f6d00c12b0af0319',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '759bbe71c0de82b1c420119ac44a51c0',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9f4aacff8879eb4896eedab5a4bfe7bd',
                '8be159aa073d908cc6bddf0d4d8a63d2',
                '9149e977c8261b698f455fc22b87c282',
                '4fd6e1e8a40c73fba602d7c47d74dbab',
                '8887132f083c424b8d8bcf1096b4b961',
                'decaf4a3d73ff970fc8dfb69cb13a1fb',
                'decaf4a3d73ff970fc8dfb69cb13a1fb',
                '360618cdf6dcf3644f015dd566f92778',
                '0d9d5f977830d94a4c5916a5f33448d8',
                'eefb81863e8cc8d52a0569c923083f27',
                '5cf83dd9774e99ba314b83500d82d6c6',
                '438ae44276dc340e746a449d6942e672',
                '2c1ea49a32108c645c2909b8d8509114',
                '33a5ffc431b49e412f7c3f5a94ad451e',
                '411051ebc57d5e68c6fcf9a8d3f6c05d',
                '6e74c63ce0963d57205c487b3acd1675',
                'a7ad32ea739d0b399aaa7eeab80532be',
                'de72c93b133e67775c1b756d55a0aa4e'
                )
        firstFrame = wsa.getSurface(wsa.size-1)
        self.pak.open("INTRO8C.WSA")
        wsa = WsaFile(self.pak.read(), self.pal, firstFrame)

        for i in xrange(wsa.size):
            surface = wsa.getSurface(i)
            self.assertEqual(knowngood[i], md5(surface.getPixels()).hexdigest())

def test_main():
    from test import test_support
    test_support.run_unittest(TestPakFile)
    test_support.run_unittest(TestCpsFile)
    test_support.run_unittest(TestEmcFile)
    test_support.run_unittest(TestMapFile)
    test_support.run_unittest(TestIcnFile)
    test_support.run_unittest(TestPalette)
    test_support.run_unittest(TestShpFile)
    test_support.run_unittest(TestSound)    
    test_support.run_unittest(TestStringFile)
    test_support.run_unittest(TestSurface)
    test_support.run_unittest(TestVocFile)
    test_support.run_unittest(TestWsaFile)

if __name__ == "__main__":
    test_main()

# vim:ts=4:sw=4:et
