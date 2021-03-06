#!/usr/bin/env python
#######################################################
# Creates a Makefile for all .cxx files in a directory.
# Uses PlotUtils and NukeCC libs and includes.
# Very self serving for the moment...
#
# May 1, 2012
# --Brian Tice (tice@physics.rutgers.edu) --
######################################################

import sys, os
from optparse import OptionParser, TitledHelpFormatter
from glob import glob

# decode argument options
usage = """
Provide a directory that contains .cxx file, where each file builds its own executable.  Create a Makefile for the directory.
LDLIBS starts as "-L$(PLOTUTILSROOT)/$(CMTCONFIG) -lplotutils", and you can add the LDLIBS line with the --ld option
INCLUDE starts as "-I$(PLOTUTILSROOT)/", and you can add to the INCLUDE line with the --include option
"""
formatter = TitledHelpFormatter( max_help_position=22, width = 190 )
parser = OptionParser( formatter = formatter, usage = usage )
parser.add_option("-d", "--dir", dest="dir", help="Directory which contains .cxx files (required)" )
parser.add_option("-o", "--output", dest="output", help="Name of the output Makefile (default=<dir>/Makefile)", default="" )
parser.add_option("--include", dest="include", help="Additional stuff for the INCLUDE line (as one string in quotes)", default="" )
parser.add_option("--ld", "--ldlibs", dest="ldlibs", help="Additional stuff for the LDLIBS line (as one string in quotes)", default="" )

if len(sys.argv) < 2 :
  parser.parse_args("--help".split())

(opts, args) = parser.parse_args()

if not os.path.isdir( opts.dir ):
  sys.exit( "ERROR: No such directory '%s'" % opts.dir )

files = glob( opts.dir + "/*.cxx" )
if not len( files ) > 0:
  sys.exit( "ERROR: No cxx files in directory '%s'" % opts.dir )

#targets are all the .cxx files without the .cxx extension
targets = [ os.path.basename( f )[:-4] for f in files ]


#add a header
makefile=\
"""
#################################################
# Makefile automatically generated by:
# %s
#################################################
""" % ' '.join(sys.argv)

#start the Makefile with common stuff
makefile +=\
"""
CXX = g++
CXXFLAGS = -g -Wall -fPIC
ROOTFLAGS = `root-config --cflags --glibs`
INCLUDE +=  -I$(UNFOLDUTILSROOT) -I$(PLOTUTILSROOT) %s
LDLIBS += -L$(UNFOLDUTILSROOT)/$(CMTCONFIG) -lRooUnfold -lUnfoldUtils -L$(PLOTUTILSROOT)/$(CMTCONFIG) -lplotutils %s

""" % ( opts.include, opts.ldlibs )

#define binaries and targets
makefile += "BINARIES = %s\n" % ( ' '.join( targets ) )
makefile += "TARGETS = %s\n\n" % ( ' '.join( [ target+".o" for target in targets ] ) )

makefile += "#--- if using 'make all' ---#\n"
makefile += "all : $(TARGETS)\n\n"

makefile += "#--- if making individual targets ---#\n"
#add build and link commands for each target
for target in targets:
  makefile +=\
"""
%s.o : %s.cxx
\t$(CXX) $(INCLUDE) $(CXXFLAGS) $(ROOTFLAGS) -o $*.o $(LDLIBS) -c $*.cxx #compile
\t$(CXX) $(INCLUDE) $(CXXFLAGS) $(ROOTFLAGS) $(LDLIBS) -o $* $*.o        #link
""" % ( target, target )

#add the clean
makefile +=\
"""
clean:
\trm -f $(BINARIES) $(TARGETS)
"""

if opts.output == "":
  opts.output = opts.dir + "/Makefile"

m = open(opts.output,"w")
m.write(makefile)
m.close()
