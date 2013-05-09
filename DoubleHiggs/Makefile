
#
# Makefile for Snowmass Delphes analysis
#
# Authors: Gena Kukartsev, John Stupak
#
# Inspired by P. Demin and ExRootAnalysis
#



include doc/Makefile.arch

ifeq ($(ARCH),macosx64)
UNDEFOPT = dynamic_lookup
endif

SrcSuf = cc

CXXFLAGS += $(ROOTCFLAGS) -Wno-write-strings -D_FILE_OFFSET_BITS=64 -DDROP_CGAL -I. -Iexternal -Iexternal/tcl
SNOWMASS_LIBS = $(shell $(RC) --libs) -lEG $(SYSLIBS)

###

SNOWMASS = libSnowmass.$(DllSuf)
SNOWMASSLIB = libSnowmass.lib

VERSION = $(shell cat VERSION)
DISTDIR = Snowmass-$(VERSION)
DISTTAR = $(DISTDIR).tar.gz

all:


SnowmassApp$(ExeSuf): \
	lib/SnowmassApp.$(ObjSuf)

lib/SnowmassApp.$(ObjSuf): \
	src/SnowmassApp.cpp

EXECUTABLE =  \
	SnowmassApp$(ExeSuf)

EXECUTABLE_OBJ =  \
	lib/SnowmassApp.$(ObjSuf)




###

all: $(EXECUTABLE)

clean:
	@rm -f core
	@rm -rf tmp

distclean: clean
	@rm -f $(EXECUTABLE)

###

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

$(EXECUTABLE_OBJ): lib/%.$(ObjSuf): src/%.cpp
	@mkdir -p $(@D)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< $(OutPutOpt)$@

$(EXECUTABLE): %$(ExeSuf): $(EXECUTABLE_OBJ)
	@echo ">> Building $@"
	@$(LD) $(LDFLAGS) $^ $(SNOWMASS_LIBS) $(OutPutOpt)$@

###


