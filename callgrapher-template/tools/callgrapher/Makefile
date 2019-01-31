##===- callgrapher/tools/callgrapher/Makefile ---------------*- Makefile -*-===##

#
# Indicate where we are relative to the top of the source tree.
#
LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=callgrapher

#
# List libraries that we'll need
#
USEDLIBS = callgraphs.a

CPPFLAGS += -std=c++11

LINK_COMPONENTS = asmparser core bitreader irreader ipo analysis support

#
# Include Makefile.common so we know what to do.
#
include $(LEVEL)/Makefile.common

