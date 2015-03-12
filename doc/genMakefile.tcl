#!/usr/bin/env tclsh

set prefix "tmp/"
set suffix " \\\n\t"

set srcSuf {.$(SrcSuf)}
set objSuf {.$(ObjSuf)}
set exeSuf {$(ExeSuf)}

proc dependencies {fileName firstLine {force 1} {command {}}} {
  global suffix headerFiles sourceFiles

  if {[info exists sourceFiles($fileName)]} return

  set sourceFiles($fileName) 1

  set list {}
  set fid [open $fileName]
  while {! [eof $fid]} {
    set line [gets $fid]
    if [regexp -- {^\s*#include\s*"((\w+/)+\w+\.(h|hh|C))"} $line] {
      set elements [split $line {"}]
      set file [lindex $elements 1]
      if [file exists $file] {
        lappend list $file
        set headerFiles($file) 1
      } elseif [file exists external/$file] {
        lappend list external/$file
        set headerFiles(external/$file) 1
	  }
    }
  }

  if {[llength $list] > 0} {
    puts -nonewline $firstLine
    foreach file $list {puts -nonewline $suffix$file}
    if {$command != {}} {
      puts {}
      puts $command
    }
    puts {}
  } elseif {$force} {
    puts -nonewline $firstLine
    if {$command != {}} {
      puts {}
      puts $command
    }
    puts {}
  }

  close $fid
}

proc dictDeps {dictVar args} {

  global prefix suffix srcSuf objSuf

  set dict [eval glob -nocomplain $args]

  set dictSrcFiles {}
  set dictObjFiles {}

  foreach fileName $dict {
    regsub {LinkDef\.h} $fileName {Dict} dictName
    set dictName $prefix$dictName

    lappend dictSrcFiles $dictName$srcSuf
    lappend dictObjFiles $dictName$objSuf

    dependencies $fileName "$dictName$srcSuf:$suffix$fileName"
  }

  puts -nonewline "${dictVar} += $suffix"
  puts [join $dictSrcFiles $suffix]
  puts {}

  puts -nonewline "${dictVar}_OBJ += $suffix"
  puts [join $dictObjFiles $suffix]
  puts {}

}

proc sourceDeps {srcPrefix args} {

  global prefix suffix srcSuf objSuf

  set source [eval glob -nocomplain $args]

  set srcObjFiles {}
  set srcObjFilesPythia8 {}

  foreach fileName $source {
    regsub {\.cc} $fileName {} srcName
    set srcObjName $prefix$srcName

    lappend srcObjFiles $srcObjName$objSuf

    dependencies $fileName "$srcObjName$objSuf:$suffix$srcName$srcSuf"
  }

  puts -nonewline "${srcPrefix}_OBJ += $suffix"
  puts [join $srcObjFiles $suffix]
  puts {}
}

proc executableDeps {args} {

  global prefix suffix objSuf exeSuf

  set executable [eval glob -nocomplain $args]

  set exeFiles {}

  foreach fileName $executable {
    regsub {\.cpp} $fileName {} exeObjName
    set exeObjName $prefix$exeObjName
    set exeName [file tail $exeObjName]

    lappend exeFiles $exeName$exeSuf
    lappend exeObjFiles $exeObjName$objSuf

    puts "$exeName$exeSuf:$suffix$exeObjName$objSuf"
    puts {}

    dependencies $fileName "$exeObjName$objSuf:$suffix$fileName"
  }

  if [info exists exeFiles] {
    puts -nonewline "EXECUTABLE += $suffix"
    puts [join $exeFiles $suffix]
    puts {}
  }
  if [info exists exeObjFiles] {
    puts -nonewline "EXECUTABLE_OBJ += $suffix"
    puts [join $exeObjFiles $suffix]
    puts {}
  }
}

proc headerDeps {} {
  global suffix headerFiles

  foreach fileName [array names headerFiles] {
    dependencies $fileName "$fileName:" 0 "\t@touch \$@"
  }
}

puts {
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

}

executableDeps {converters/*.cpp} {examples/*.cpp}

dictDeps {ILCANALYSIS_DICT} {classes/ClassesLinkDef.h} {ExRootAnalysis/ExRootAnalysisLinkDef.h}

sourceDeps {ILCANALYSIS} {classes/*.cc} {ExRootAnalysis/*.cc}

headerDeps

puts {

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

}
