
TOP = $(AMREX_HOME)
#
# Variables for the user to set ...
#
PRECISION     = DOUBLE
DEBUG	      = TRUE
DEBUG	      = FALSE
DIM	      = 2
COMP          = g++
USE_MPI       = TRUE 
#
# Base name of the executable.
#
#EBASE = Add
#EBASE = ComputeAmrDataNorms
#EBASE = DiffFab
#EBASE = DiffSameDomainRefined
#EBASE = DiffSameDomainRefinedComposite
#EBASE = DiffSameDomainRefinedStag
#EBASE = DiffSameGrid2
#EBASE = DiffSameGrid
#EBASE = DiffSameGridRefined
#EBASE = DiffUniform
#EBASE = PltFileNormB
#EBASE = PltFileNorm
#EBASE = PltFileScalConvRate

CEXE_sources += $(EBASE).cpp

include $(AMREX_HOME)/Tools/GNUMake/Make.defs

DEFINES += -DBL_NOLINEVALUES -DBL_PARALLEL_IO

include Make.package
include ../Make.package
include $(AMREX_HOME)/Src/Base/Make.package
include $(AMREX_HOME)/Src/Boundary/Make.package
#include $(AMREX_HOME)/Src/Amr/Make.package
include $(AMREX_HOME)/Src/Extern/amrdata/Make.package

INCLUDE_LOCATIONS += .
INCLUDE_LOCATIONS += ..
vpathdir += ..

INCLUDE_LOCATIONS += $(AMREX_HOME)/Src/Base
vpathdir += $(AMREX_HOME)/Src/Base

INCLUDE_LOCATIONS += $(AMREX_HOME)/Src/Boundary
vpathdir += $(AMREX_HOME)/Src/Boundary

INCLUDE_LOCATIONS += $(AMREX_HOME)/Src/Amr
vpathdir += $(AMREX_HOME)/Src/Amr

INCLUDE_LOCATIONS += $(AMREX_HOME)/Src/Extern/amrdata
vpathdir += $(AMREX_HOME)/Src/Extern/amrdata

CEXE_sources += BCRec.cpp
CEXE_headers += BCRec.H BC_TYPES.H

vpath %.c   : . $(vpathdir)
vpath %.h   : . $(vpathdir)
vpath %.cpp : . $(vpathdir)
vpath %.H   : . $(vpathdir)
vpath %.F   : . $(vpathdir)
vpath %.f   : . $(vpathdir)
vpath %.f90 : . $(vpathdir)

all: $(executable)

include $(AMREX_HOME)/Tools/GNUMake/Make.rules

#include $(TOP)/Tools/GNUMake/Make.defs ./Make.package

#ifeq ($(USE_ARRAYVIEW),TRUE)
#  DEFINES += -DBL_USE_ARRAYVIEW
#  DEFINES += -DBL_ARRAYVIEW_TAGBOX
#endif





