#!/bin/env python

from optparse import *
from pyeastwood import *
from os.path import exists

def openFile(filename):
    fname = filename.split(":")
    if len(fname) == 2:
        f = PakFile(cpsfile[0])
        f.open(cpsfile[1])
    else:
        f = open(cpsfile[0])
    return f


def main():
    usage = "usage: %prog [options] arg"
    palette = None
    surface = None

    parser = OptionParser(usage)

    pakOptions = OptionGroup(parser, "PAK options", "Options for PAK files")
    pakOptions.add_option("-P", "--pak", help="PAK", dest="pakfile")
    pakOptions.add_option("-l", "--list", action="store_true", dest="listfiles",
            help="list files", default=False)
    pakOptions.add_option("-x", "--extract", dest="extract",
            help="extract file")
    pakOptions.add_option("-a", "--add", dest="addfile",
            help="add file")
    parser.add_option_group(pakOptions)

    cpsOptions = OptionGroup(parser, "PAL options", "Options for PAL files")
    cpsOptions.add_option("-p", "--pal", help="PAL", dest="palfile")
    parser.add_option_group(cpsOptions);

    cpsOptions = OptionGroup(parser, "CPS options", "Options for CPS files")
    cpsOptions.add_option("-C", "--cps", help="CPS", dest="cpsfile")
    parser.add_option_group(cpsOptions);

    shpOptions = OptionGroup(parser, "SHP options", "Options for SHP files")
    shpOptions.add_option("-S", "--shp", help="SHP", dest="shpfile")
    shpOptions.add_option("--size", help="Size in format pictures WxH", dest="shpsize")
    shpOptions.add_option("--tiles", help="What tiles to use, a list of indexes in the form n,n,...,n", dest="tiles")
    parser.add_option_group(shpOptions);

    wsaOptions = OptionGroup(parser, "WSA options", "Options for WSA files")
    wsaOptions.add_option("-W", "--wsa", help="WSA", dest="wsafile")
    wsaOptions.add_option("--all", help="Save all frames to several files in format ALL-n.bmp", dest="all")    
    parser.add_option_group(wsaOptions);



    emcOptions = OptionGroup(parser, "EMC options", "Options for EMC files")
    emcOptions.add_option("-E", "--emc", help="EMC", dest="emcfile")
    emcOptions.add_option("--assemble", action="store_true", default=False, dest="assemble")
    emcOptions.add_option("--disassemble", action="store_true", default=False, dest="disassemble")
    parser.add_option_group(emcOptions);


    parser.add_option("-o", "--output", dest="output",
            help="save to OUTPUT")

    parser.add_option("-i", "--index", dest="index",
            help="get item at INDEX")

    parser.add_option("--scale", dest="scale", help="Scale graphics using" \
            "either 2x, 2x3, 2x4, 3x or 4x")


    (options, args) = parser.parse_args()
    if options.palfile:
        f = openFile(options.palfile)
        palette = PalFile(f.read()).getPalette()

    if options.pakfile:
        if not exists(options.pakfile) and options.addfile:
            pak = PakFile(options.pakfile, create=True)
        else:
            pak = PakFile(options.pakfile)

        if options.addfile:
            pak.open(options.addfile, "w")
            f = open(options.addfile, "r")
            pak.write(f.read())
            f.close()
            pak.close()

        elif options.extract and options.output:
            pak = PakFile(options.pakfile)
            pak.open(options.extract)
            out = open(options.output, "w")
            out.write(pak.read())
            out.close()
            pak.close()

        elif options.listfiles:
            pak = PakFile(options.pakfile)
            for f in pak.listfiles():
                print f
    
    elif options.cpsfile:
        f = openFile(options.cpsfile)
        if palette:
            cps = CpsFile(f.read(), palette)
        else:
            cps = CpsFile(f.read())
        f.close()

        surface = cps.getSurface()

    elif options.shpfile:
        if not palette:
            parser.error("A palette is required")
        if not options.index or (options.shpfile and options.tiles):
            parser.error("An index or size & tiles list is required")

        f = openFile(options.shpfile)
        shp = ShpFile(f.read(), palette)
        f.close()

        if options.index:
            surface = shp.getSurface(options.index)
        elif options.shpsize and options.tiles:
            w, h = options.shpsize.split("x")
            tiles = options.tiles.split(",")
            for i in xrange(len(tiles)):
                tiles[i] = int(tiles[i])
            surface = shp.getTiles(w, h, tiles)

    elif options.wsafile:
        if not palette:
            parser.error("A palette is required")
        if not options.index or options.all:
            parser.error("An index or --all argument is required")

        f = openFile(options.shpfile)
        wsa = WsaFile(f.read(), palette)
        f.close()

        if options.index:
            surface = wsa.getSurface(options.index)
        elif options.all:
            for i in xrange(wsa.size):
                f = os.open("%-%.2d.bmp", "w")
                f.write(wsa.getSurface(i).saveBMP())
                f.close()

    elif options.emcfile:
        f = openFile(options.emcfile)
        emc = None
        if options.assemble:
            emc = EmcFile(f.read(), 'a')
        elif options.disassemble:
            emc = EmcFile(f.read(), 'd')
        f.close()
        out = open(options.output, "w")
        out.write(emc.get())
        out.close()


    if surface and options.output:
        if options.scale:
            scaler = None
            if options.scale == "2x":
                scaler = Scale2X
            elif options.scale == "2x3":
                scaler = Scale2X3
            elif options.scale == "2x4":
                scaler = Scale2X4
            elif options.scale == "3x":
                scaler = Scale3X
            elif options.scale == "4x":
                scaler = Scale4X
            else:
                parser.error("Invalid scaler: %s" % options.scale)
            surface = surface.getScaled(scaler)
        out = open(options.output, "w")
        out.write(surface.saveBMP())
        out.close()


if __name__ == "__main__":
    main()

# vim:ts=4:sw=4:et
