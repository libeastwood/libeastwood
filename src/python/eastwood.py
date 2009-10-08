#!/bin/env python

from optparse import OptionParser
from pyeastwood import *

def main():
    usage = "usage: %prog [options] arg"
    pak = None
    palette = None
    parser = OptionParser(usage)
    parser.add_option("-o", "--output", dest="output",
	    help="save to OUTPUT")
    parser.add_option("-P", "--pak", dest="pakfile",
	    help="open files from PAKFILE")
    parser.add_option("-l", "--list", action="store_true", dest="listfiles",
	    help="list files", default=False)
    parser.add_option("-f", "--file", dest="filename",
	    help="read data from FILENAME")
    parser.add_option("-p", "--palette", dest="palette",
	    help="use palette from PALETTE")
    parser.add_option("-c", "--cps", dest="cps",
	    help="load CPS file from CPS")
    parser.add_option("-a", "--assemble", dest="assemble",
	    help="assemble EMC file from ASSEMBLE")
    parser.add_option("-d", "--disassemble", dest="disassemble",
	    help="disassemble EMC file from DISASSEMBLE")

    (options, args) = parser.parse_args()
    if options.pakfile:
	pak = PakFile(options.pakfile)

    if options.pakfile and options.listfiles:
	print pak.listfiles()

    if options.pakfile and options.filename and options.output:
	pak.open(options.filename)
	out = open(options.output, "w")
	out.write(pak.read())
	out.close()

    if options.palette:
	if pak:
	    pak.open(options.palette)
	    f = pak
	else:
	    f = os.open(options.palette)
	palfile = PalFile(f.read())
	palette = palfile.getPalette()
	f.close()

    if options.cps and options.output:
	if pak:
	    pak.open(options.cps)
	    f = pak
	else:
	    f = open(options.cps)
	if palette:
	    cps = CpsFile(f.read(), palette)
	else:
	    cps = CpsFile(f.read())
	f.close()
	o = open(options.output, "w")
	o.write(cps.getSurface().saveBMP())
	o.close()

    if options.assemble and options.output:
	if pak:
	    pak.open(options.assemble)
	    f = pak
	else:
	    f = open(options.assemble)
	assemble = EmcFile(f.read(), EMC_ASSEMBLE)
	f.close()
	o = open(options.output, "w")
	o.write(assemble.get())
	o.close()

    if options.disassemble and options.output:
	if pak:
	    pak.open(options.disassemble)
	    f = pak
	else:
	    f = open(options.disassemble)
	disassemble = EmcFile(f.read(), EMC_DISASSEMBLE)
	f.close()
	o = open(options.output, "w")
	o.write(disassemble.get())
	o.close()

if __name__ == "__main__":
    main()

