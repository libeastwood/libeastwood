#!/bin/env python

import sys
from optparse import *
from pyeastwood import *
from os.path import exists

def openFile(filename):
    fname = filename.split(":")
    if len(fname) == 2:
        f = PakFile(fname[0])
        f.open(fname[1])
    else:
        f = open(fname[0])
    return f


class SubOptionParser(OptionParser):
    def __init__(self, *args, **kwargs):
        usage = "usage: %%prog %s [options] arg" % sys.argv[1]
        OptionParser.__init__(self, usage, *args, **kwargs)
        self.add_option("-o", "--output", dest="output", help="save to OUTPUT")

    def parse_args(self, args):
        if args[1] == '-h' or args[1] == '--help':
            args.pop(0)
        return OptionParser.parse_args(self, args)

    def process(self):
        (self.options, self.args) = self.parse_args(sys.argv[1:])

    def postProcess(self):
        pass

class SurfaceOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        self.add_option("-p", "--pal", help="PAL", dest="palfile")
        self.add_option("--scale", dest="scale", help="Scale graphics using " \
                "either 2x, 2x3, 2x4, 3x or 4x")
        self.palette = None
        self.surface = None

    def process(self):
        SubOptionParser.process(self)

        if self.options.palfile:
            f = openFile(self.options.palfile)
            self.palette = PalFile(f.read()).getPalette()
            f.close()
        else:
            self.error("A palette is required")

    def postProcess(self):
        SubOptionParser.postProcess(self)
        if self.surface and self.options.output:
            if self.options.scale:
                scaler = None
                if self.options.scale == "2x":
                    scaler = Scale2X
                elif self.options.scale == "2x3":
                    scaler = Scale2X3
                elif self.options.scale == "2x4":
                    scaler = Scale2X4
                elif self.options.scale == "3x":
                    scaler = Scale3X
                elif self.options.scale == "4x":
                    scaler = Scale4X
                else:
                    parser.error("Invalid scaler: %s" % self.options.scale)
                self.surface = self.surface.getScaled(scaler)
            out = open(self.options.output, "w")
            out.write(self.surface.saveBMP())
            out.close()

class PakOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--pak", help="PAK", dest="pakfile")
        self.add_option("-l", "--list", action="store_true", dest="listfiles",
                help="list files", default=False)
        self.add_option("-x", "--extract", dest="extract",
                help="extract file")
        self.add_option("-a", "--add", dest="addfile",
                help="add file")

    def process(self):
        SubOptionParser.process(self)
        
        if not exists(self.options.pakfile) and self.options.addfile:
            pak = PakFile(self.options.pakfile, create=True)
        else:
            pak = PakFile(self.options.pakfile)

        if self.options.addfile:
            pak.open(self.options.addfile, "w")
            f = open(options.addfile, "r")
            pak.write(f.read())
            f.close()
            pak.close()
    
        elif self.options.extract and self.options.output:
            pak = PakFile(self.options.pakfile)
            pak.open(self.options.extract)
            out = open(self.options.output, "w")
            out.write(pak.read())
            out.close()
            pak.close()
        
        elif self.options.listfiles:
            pak = PakFile(self.options.pakfile)
            for f in pak.listfiles():
                print f

class CpsOptionParser(SurfaceOptionParser):
    def __init__(self, *args, **kwargs):
        SurfaceOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--cps", help="CPS", dest="cpsfile")

    def process(self):
        SurfaceOptionParser.process(self)

        f = openFile(self.options.cpsfile)
        if self.palette:
            cps = CpsFile(f.read(), self.palette)
        else:
            cps = CpsFile(f.read())
        f.close()

        self.surface = cps.getSurface()

class ShpOptionParser(SurfaceOptionParser):
    def __init__(self, *args, **kwargs):
        SurfaceOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--shp", help="SHP", dest="shpfile")
        self.add_option("-i", "--index", dest="index",
                help="get surface at INDEX")
        self.add_option("--size", help="Size in format pictures WxH", dest="size")
        self.add_option("--tiles", help="What tiles to use, a list of indexes in the form n,n,...,n",
                dest="tiles")

    def process(self):
        SurfaceOptionParser.process(self)

        if not self.options.index or (self.options.size and self.options.tiles):
            parser.error("An index or size & tiles list is required")

        f = openFile(self.options.shpfile)
        shp = ShpFile(f.read(), self.palette)
        f.close()

        if self.options.index:
            self.surface = shp.getSurface(self.options.index)
        elif self.options.size and self.options.tiles:
            w, h = self.options.size.split("x")
            tiles = self.options.tiles.split(",")
            for i in xrange(len(tiles)):
                tiles[i] = int(tiles[i])
            self.surface = shp.getTiles(w, h, tiles)

class WsaOptionParser(SurfaceOptionParser):
    def __init__(self, *args, **kwargs):
        SurfaceOptionParser.__init__(self, *args, **kwargs)
        self.add_option("-W", "--wsa", help="WSA", dest="wsafile")
        self.add_option("-i", "--index", dest="index",
                help="get surface at INDEX")
        self.add_option("--all", help="Save all frames to several files in format ALL-n.bmp", dest="all")    

    def process(self):
        SurfaceOptionParser.process(self)

        if not self.options.index or self.options.all:
            parser.error("An index or --all argument is required")

        f = openFile(self.options.shpfile)
        wsa = WsaFile(f.read(), self.palette)
        f.close()

        if self.options.index:
            self.surface = wsa.getSurface(self.options.index)
        elif self.options.all:
            for i in xrange(wsa.size):
                f = os.open("%-%.2d.bmp", "w")
                f.write(wsa.getSurface(i).saveBMP())
                f.close()

class EmcOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--emc", help="EMC", dest="emcfile")
        self.add_option("--assemble", action="store_true", default=False, dest="assemble")
        self.add_option("--disassemble", action="store_true", default=False, dest="disassemble")


    def process(self):
        SubOptionParser.process(self)

        f = openFile(self.options.emcfile)
        emc = None
        if self.options.assemble:
            emc = EmcFile(f.read(), 'a')
        elif self.options.disassemble:
            emc = EmcFile(f.read(), 'd')
        else:
            self.error("need to specify either --assemble or --disassemble")
        f.close()
        out = open(self.options.output, "w")
        out.write(emc.get())
        out.close()

def main():
    usage = "usage: %prog [options] arg"
    palette = None
    surface = None

    parser = OptionParser(usage)

    parser.add_option("--pak", dest="pak", action="store_true", default=False,
            help="Options for PAK files")
    parser.add_option("--cps", dest="cps", action="store_true", default=False,
            help="Options for CPS files")
    parser.add_option("--shp", dest="shp", action="store_true", default=False,
            help="Options for SHP files")
    parser.add_option("--wsa", dest="wsa", action="store_true", default=False,
            help="Options for WSA files")
    parser.add_option("--emc", dest="emc", action="store_true", default=False,
            help="Options for EMC files")


    if len(sys.argv) == 1:
        locargs = ["--help"]
    else:
        locargs = [sys.argv[1]]
    (options, args) = parser.parse_args(locargs)

    subParser = None
    if options.pak:
        subParser = PakOptionParser()
    elif options.cps:
        subParser = CpsOptionParser()
    elif options.shp:
        subParser = ShpOptionParser()
    elif options.wsa:
        subParser = WsaOptionParser()
    elif options.emc:
        subParser = EmcOptionParser()
    else:
        error("Invalid option: %s" % locargs[0])

    if subParser:
        subParser.process()
        subParser.postProcess()
        

if __name__ == "__main__":
    main()

# vim:ts=4:sw=4:et
