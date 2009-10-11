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
        if len(args) == 1 or args[1] == '-h' or args[1] == '--help':
            args.pop(0)
        if len(args) == 0:
            args.append("--help")
        return OptionParser.parse_args(self, args)

    def process(self):
        (self.options, self.args) = self.parse_args(sys.argv[1:])

    def postProcess(self):
        pass

class SoundOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        resample = OptionGroup(self, "Resampling options")
        resample.add_option("--resample", dest="resample", help="Resample sound " \
                "using either 'linear', 'zero_order_hold', 'sinc_fastest', 'sinc_medium_quality', or 'sinc_best_quality'")
        resample.add_option("-f", "--frequency", type="int", dest="frequency", help="Resample to FREQUENCY hz");
        resample.add_option("-c", "--channels", type="int", dest="channels", help="Resample to CHANNELS channels")
        resample.add_option("-F", "--format", dest="format", help="Resample to FORMAT format, " \
                "either u8, s16be or s16le")
        self.add_option_group(resample)
        self.sound = None

    def postProcess(self):
        SubOptionParser.postProcess(self)
        if self.sound and self.options.output:
            if self.options.resample:
                if self.options.channels:
                    channels = self.options.channels
                else:
                    channels = self.sound.channels
                if self.options.frequency:
                    frequency = self.options.frequency
                else:
                    frequency = self.sound.frequency
                format = self.sound.format
                if self.options.format:
                   if self.options.format == "u8":
                       format = FMT_U8
                   elif self.options.format == "s16be":
                       format = FMT_S16BE
                   elif self.options.format == "s16le":
                       format = FMT_S16LE
                   else:
                       self.error("Unsupported format: %s" % self.options.format)
                interpolator = None
                if self.options.resample == "linear":
                    interpolator = I_LINEAR
                elif self.options.resample == "zero_order_hold":
                    interpolator = I_ZERO_ORDER_HOLD
                elif self.options.resample == "sinc_fastest":
                    interpolator = I_SINC_FASTEST
                elif self.options.resample == "sinc_medium_quality":
                    interpolator = I_SINC_MEDIUM_QUALITY
                elif self.options.resample == "sinc_best_quality":
                    interpolator = I_SINC_BEST_QUALITY
                else:
                    parser.error("Invalid resampler: %s" % self.options.resample)
                self.sound = self.sound.getResampled(channels, frequency, format, interpolator)
            out = open(self.options.output, "w")
            out.write(self.sound.saveWAV())
            out.close()

class SurfaceOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        self.add_option("-p", "--pal", help="PAL", dest="palfile")
        self.add_option("--scale", dest="scale", help="Scale graphics using " \
                "either '2x', '2x3', '2x4', '3x' or '4x'")
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

class IcnOptionParser(SurfaceOptionParser):
    def __init__(self, *args, **kwargs):
        SurfaceOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--icn", help="ICN", dest="icnfile")
        self.add_option("-m", "--map", help="Tile map", dest="mapfile")
        self.add_option("-i", "--index", type="int", dest="index",
                help="get surface at INDEX")
        self.add_option("-t", "--tiles", help="Get tiled surface from map index TILES",
                dest="tiles")
        self.add_option("-f", "--framebyframe", action="store_true", default=False,
                help="Create all frames to use for animation", dest="framebyframe")


    def process(self):
        SurfaceOptionParser.process(self)

        if self.options.mapfile:
            f = openFile(self.options.mapfile)
            self.map = MapFile(f.read())
            f.close()
        else:
            self.error("Tile map (--map) is required")

        if not (self.options.index or self.options.tiles):
            self.error("An index or size & tiles list is required")

        f = openFile(self.options.icnfile)
        icn = IcnFile(f.read(), self.map, self.palette)
        f.close()

        if self.options.index:
            self.surface = icn.getSurface(self.options.index)
        elif self.options.tiles:
            self.surface = icn.getTiles(self.options.tiles, self.options.framebyframe)

class ShpOptionParser(SurfaceOptionParser):
    def __init__(self, *args, **kwargs):
        SurfaceOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--shp", help="SHP", dest="shpfile")
        self.add_option("-i", "--index", type="int", dest="index",
                help="get surface at INDEX")
        self.add_option("--size", help="Size in format pictures WxH", dest="size")
        self.add_option("--tiles", help="What tiles to use, a list of indexes in the form n,n,...,n",
                dest="tiles")

    def process(self):
        SurfaceOptionParser.process(self)

        if not (self.options.index or (self.options.size and self.options.tiles)):
            self.error("An index or size & tiles list is required")

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
        self.add_option("-i", "--index", type="int", dest="index",
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

class StringOptionParser(SubOptionParser):
    def __init__(self, *args, **kwargs):
        SubOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--string", help="String", dest="stringfile")
        self.add_option("-i", "--index", type="int", dest="index",
                help="get string at INDEX")
        self.add_option("-t", "--type", dest="type",
                help="get mission string of specific type, either 'advice', 'description', 'lose' or 'win'")
        self.add_option("-n", action="store_true", default=False,
                dest="size", help="get number of strings")

    def process(self):
        SubOptionParser.process(self)

        f = openFile(self.options.stringfile)
        str = StringFile(f.read())
        f.close()

        if self.options.size:
            print str.size
        elif self.options.index:
            index = self.options.index
            if self.options.type:
                type = None
                if self.options.type == 'advice':
                    type = MISSION_ADVICE
                elif self.options.type == 'description':
                    type = MISSION_DESCRIPTION
                elif self.options.type == 'lose':
                    type = MISSION_LOSE
                elif self.options.type == 'win':
                    type = MISSION_WIN
                else:
                    self.error("Invalid mission type: %s" % self.options.type)
                print str.getMissionString(index, type)
            else:
                print str.getString(index)

class VocOptionParser(SoundOptionParser):
    def __init__(self, *args, **kwargs):
        SoundOptionParser.__init__(self, *args, **kwargs)
        self.add_option("--voc", help="VOC", dest="vocfile")

    def process(self):
        SoundOptionParser.process(self)

        f = openFile(self.options.vocfile)
        voc = VocFile(f.read())
        f.close()

        self.sound = voc.getSound()

def main():
    usage = "usage: %prog [options] arg"
    palette = None
    surface = None

    parser = OptionParser(usage)

    parser.add_option("--pak", dest="pak", action="store_true", default=False,
            help="Options for PAK files")
    parser.add_option("--cps", dest="cps", action="store_true", default=False,
            help="Options for CPS files")
    parser.add_option("--icn", dest="icn", action="store_true", default=False,
            help="Options for ICN files")
    parser.add_option("--shp", dest="shp", action="store_true", default=False,
            help="Options for SHP files")
    parser.add_option("--wsa", dest="wsa", action="store_true", default=False,
            help="Options for WSA files")
    parser.add_option("--emc", dest="emc", action="store_true", default=False,
            help="Options for EMC files")
    parser.add_option("--string", dest="string", action="store_true", default=False,
            help="Options for string files")
    parser.add_option("--voc", dest="voc", action="store_true", default=False,
            help="Options for VOC files")

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
    elif options.icn:
        subParser = IcnOptionParser()
    elif options.shp:
        subParser = ShpOptionParser()
    elif options.wsa:
        subParser = WsaOptionParser()
    elif options.emc:
        subParser = EmcOptionParser()
    elif options.string:
        subParser = StringOptionParser()
    elif options.voc:
        subParser = VocOptionParser()
    else:
        error("Invalid option: %s" % locargs[0])

    if subParser:
        subParser.process()
        subParser.postProcess()
        

if __name__ == "__main__":
    main()

# vim:ts=4:sw=4:et
