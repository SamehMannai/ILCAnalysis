
#
# Makefile for ExRootAnalysis
#
# Author: P. Demin - UCL, Louvain-la-Neuve
#
# multi-platform configuration is taken from ROOT (root/test/Makefile.arch)
#

include doc/Makefile.arch

ifeq ($(ARCH),macosx64)
UNDEFOPT = dynamic_lookup
endif

SrcSuf = cc

CXXFLAGS += $(ROOTCFLAGS) -Wno-write-strings -D_FILE_OFFSET_BITS=64 -DDROP_CGAL -I. -I/nfs/soft/ilc/v01-17-04/lcio/v02-04-03/include
ILCANALYSIS_LIBS = $(shell $(RC) --libs) -lEG $(SYSLIBS) -L/nfs/soft/ilc/v01-17-04/lcio/v02-04-03/lib -llcio

###

ILCANALYSIS = libILCAnalysis.$(DllSuf)
ILCANALYSISLIB = libILCAnalysis.lib

VERSION = $(shell cat VERSION)
DISTDIR = ILCAnalysis-$(VERSION)
DISTTAR = $(DISTDIR).tar.gz

all:


streamout2root_2014$(ExeSuf): \
	tmp/converters/streamout2root_2014.$(ObjSuf)

tmp/converters/streamout2root_2014.$(ObjSuf): \
	converters/streamout2root_2014.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	classes/TCalorimeterHit.h
raw2root$(ExeSuf): \
	tmp/converters/raw2root.$(ObjSuf)

tmp/converters/raw2root.$(ObjSuf): \
	converters/raw2root.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	classes/TCalorimeterHit.h \
	classes/DIFUnpacker.h
sim2root$(ExeSuf): \
	tmp/converters/sim2root.$(ObjSuf)

tmp/converters/sim2root.$(ObjSuf): \
	converters/sim2root.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h
trivent2root$(ExeSuf): \
	tmp/converters/trivent2root.$(ObjSuf)

tmp/converters/trivent2root.$(ObjSuf): \
	converters/trivent2root.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h
raw2root_2014$(ExeSuf): \
	tmp/converters/raw2root_2014.$(ObjSuf)

tmp/converters/raw2root_2014.$(ObjSuf): \
	converters/raw2root_2014.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	classes/TCalorimeterHit.h \
	classes/DIFUnpacker.h
SelectionSam$(ExeSuf): \
	tmp/examples/SelectionSam.$(ObjSuf)

tmp/examples/SelectionSam.$(ObjSuf): \
	examples/SelectionSam.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeReader.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	ExRootAnalysis/ExRootResult.h \
	ExRootAnalysis/ExRootUtilities.h \
	ExRootAnalysis/ExRootProgressBar.h \
	examples/SelectionSam.C
SelectionSamData$(ExeSuf): \
	tmp/examples/SelectionSamData.$(ObjSuf)

tmp/examples/SelectionSamData.$(ObjSuf): \
	examples/SelectionSamData.cpp \
	classes/ILCClasses.h \
	ExRootAnalysis/ExRootTreeReader.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	ExRootAnalysis/ExRootResult.h \
	ExRootAnalysis/ExRootUtilities.h \
	ExRootAnalysis/ExRootProgressBar.h \
	examples/SelectionSamData.C
EXECUTABLE +=  \
	streamout2root_2014$(ExeSuf) \
	raw2root$(ExeSuf) \
	sim2root$(ExeSuf) \
	trivent2root$(ExeSuf) \
	raw2root_2014$(ExeSuf) \
	SelectionSam$(ExeSuf) \
	SelectionSamData$(ExeSuf)

EXECUTABLE_OBJ +=  \
	tmp/converters/streamout2root_2014.$(ObjSuf) \
	tmp/converters/raw2root.$(ObjSuf) \
	tmp/converters/sim2root.$(ObjSuf) \
	tmp/converters/trivent2root.$(ObjSuf) \
	tmp/converters/raw2root_2014.$(ObjSuf) \
	tmp/examples/SelectionSam.$(ObjSuf) \
	tmp/examples/SelectionSamData.$(ObjSuf)

tmp/classes/ClassesDict.$(SrcSuf): \
	classes/ClassesLinkDef.h \
	classes/SortableObject.h \
	classes/ILCClasses.h
tmp/ExRootAnalysis/ExRootAnalysisDict.$(SrcSuf): \
	ExRootAnalysis/ExRootAnalysisLinkDef.h \
	ExRootAnalysis/ExRootTreeReader.h \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h \
	ExRootAnalysis/ExRootResult.h \
	ExRootAnalysis/ExRootUtilities.h \
	ExRootAnalysis/ExRootClassifier.h \
	ExRootAnalysis/ExRootFilter.h \
	ExRootAnalysis/ExRootProgressBar.h
ILCANALYSIS_DICT +=  \
	tmp/classes/ClassesDict.$(SrcSuf) \
	tmp/ExRootAnalysis/ExRootAnalysisDict.$(SrcSuf)

ILCANALYSIS_DICT_OBJ +=  \
	tmp/classes/ClassesDict.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootAnalysisDict.$(ObjSuf)

tmp/classes/ILCClasses.$(ObjSuf): \
	classes/ILCClasses.$(SrcSuf) \
	classes/ILCClasses.h \
	classes/SortableObject.h
tmp/classes/DIFUnpacker.$(ObjSuf): \
	classes/DIFUnpacker.$(SrcSuf)
tmp/ExRootAnalysis/ExRootTreeWriter.$(ObjSuf): \
	ExRootAnalysis/ExRootTreeWriter.$(SrcSuf) \
	ExRootAnalysis/ExRootTreeWriter.h \
	ExRootAnalysis/ExRootTreeBranch.h
tmp/ExRootAnalysis/ExRootProgressBar.$(ObjSuf): \
	ExRootAnalysis/ExRootProgressBar.$(SrcSuf) \
	ExRootAnalysis/ExRootProgressBar.h
tmp/ExRootAnalysis/ExRootUtilities.$(ObjSuf): \
	ExRootAnalysis/ExRootUtilities.$(SrcSuf) \
	ExRootAnalysis/ExRootUtilities.h
tmp/ExRootAnalysis/ExRootTreeReader.$(ObjSuf): \
	ExRootAnalysis/ExRootTreeReader.$(SrcSuf) \
	ExRootAnalysis/ExRootTreeReader.h
tmp/ExRootAnalysis/ExRootTreeBranch.$(ObjSuf): \
	ExRootAnalysis/ExRootTreeBranch.$(SrcSuf) \
	ExRootAnalysis/ExRootTreeBranch.h
tmp/ExRootAnalysis/ExRootFilter.$(ObjSuf): \
	ExRootAnalysis/ExRootFilter.$(SrcSuf) \
	ExRootAnalysis/ExRootFilter.h \
	ExRootAnalysis/ExRootClassifier.h
tmp/ExRootAnalysis/ExRootResult.$(ObjSuf): \
	ExRootAnalysis/ExRootResult.$(SrcSuf) \
	ExRootAnalysis/ExRootResult.h \
	ExRootAnalysis/ExRootUtilities.h
ILCANALYSIS_OBJ +=  \
	tmp/classes/ILCClasses.$(ObjSuf) \
	tmp/classes/DIFUnpacker.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootTreeWriter.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootProgressBar.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootUtilities.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootTreeReader.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootTreeBranch.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootFilter.$(ObjSuf) \
	tmp/ExRootAnalysis/ExRootResult.$(ObjSuf)

classes/ILCClasses.h: \
	classes/SortableObject.h
	@touch $@



###

all: $(ILCANALYSIS) $(EXECUTABLE)

$(ILCANALYSIS): $(ILCANALYSIS_DICT_OBJ) $(ILCANALYSIS_OBJ)
	@mkdir -p $(@D)
	@echo ">> Building $@"
ifeq ($(ARCH),aix5)
	@$(MAKESHARED) $(OutPutOpt) $@ $(ILCANALYSIS_LIBS) -p 0 $^
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
	@$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(ILCANALYSIS_LIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
	@ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
	@bindexplib $* $^ > $*.def
	@lib -nologo -MACHINE:IX86 $^ -def:$*.def $(OutPutOpt)$(ILCANALYSISLIB)
	@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $*.exp $(ILCANALYSIS_LIBS) $(OutPutOpt)$@
	@$(MT_DLL)
else
	@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(ILCANALYSIS_LIBS)
	@$(MT_DLL)
endif
endif
endif

clean:
	@rm -f $(ILCANALYSIS_DICT_OBJ) $(ILCANALYSIS_OBJ) core
	@rm -rf tmp

distclean: clean
	@rm -f $(ILCANALYSIS) $(ILCANALYSISLIB) $(EXECUTABLE)

dist:
	@echo ">> Building $(DISTTAR)"
	@mkdir -p $(DISTDIR)
	@cp -a CHANGELOG CREDITS README VERSION Makefile configure classes converters display doc examples external modules python readers $(DISTDIR)
	@find $(DISTDIR) -depth -name .\* -exec rm -rf {} \;
	@tar -czf $(DISTTAR) $(DISTDIR)
	@rm -rf $(DISTDIR)

###

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

%Dict.$(SrcSuf):
	@mkdir -p $(@D)
	@echo ">> Generating $@"
	@rootcint -f $@ -c -Iexternal $<
	@echo "#define private public" > $@.arch
	@echo "#define protected public" >> $@.arch
	@mv $@ $@.base
	@cat $@.arch $< $@.base > $@
	@rm $@.arch $@.base

$(ILCANALYSIS_OBJ): tmp/%.$(ObjSuf): %.$(SrcSuf)
	@mkdir -p $(@D)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< $(OutPutOpt)$@

$(ILCANALYSIS_DICT_OBJ): %.$(ObjSuf): %.$(SrcSuf)
	@mkdir -p $(@D)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< $(OutPutOpt)$@

$(EXECUTABLE_OBJ): tmp/%.$(ObjSuf): %.cpp
	@mkdir -p $(@D)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< $(OutPutOpt)$@

$(EXECUTABLE): %$(ExeSuf): $(ILCANALYSIS_DICT_OBJ) $(ILCANALYSIS_OBJ)
	@echo ">> Building $@"
	@$(LD) $(LDFLAGS) $^ $(ILCANALYSIS_LIBS) $(OutPutOpt)$@

###


