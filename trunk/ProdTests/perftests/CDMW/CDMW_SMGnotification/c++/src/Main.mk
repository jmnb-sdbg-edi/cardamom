#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI. All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *

SHELL = /bin/sh

include ../site.mk
# Imported variables
#   
#   CDMW_HOME
#   RM
#   CXX
#   CXXLD
#   IDLMKDEPEND
#   IDLMKINCLUDE
#   ORB
#   IDL2CXX
#   
#   CPPFLAGS_ORB
#   CXXFLAGS_ORB
#   IDLFLAGS_ORB
#   LDFLAGS_ORB
#   LIBS_ORB
#   
#   CPPFLAGS_CDMW
#   CXXFLAGS_CDMW
#   LDFLAGS_CDMW
#   IDLFLAGS_CDMW
#   LIBS_CDMW
#   
# NOTA: The variables below includes CDMW/ORB/OS specific variables
#   CPPFLAGS
#   CXXFLAGS
#   LDFLAGS
#   IDLFLAGS
#   LIBS
include ../config.mk

#Append to CPPFLAGS and IDLFLAGS
override CPPFLAGS += -I. -I../include -I../generated \
        $(foreach d, $(shell find $(CDMW_HOME)/include/c++ -type d), -I$(d))
override CPPFLAGS += -I../../../../cTools/src  -I$(TAO_ROOT)/orbsvcs/orbsvcs
override IDLFLAGS += -I../idl -I../generated

ALL_LIBS=$(LIBS)

CTOOLS_LIBS=#-L../../../../cTools/lib -lCTools

# Exported variables
#   CORBA_ALL_IDL,
#   CORBA_CL_ALL_SRC,
#   CORBA_SRV_ALL_SRC,
#   CORBA_CL_ALL_OBJS,
#   CORBA_SRV_ALL_OBJS,
#   CORBA_CL_ALL_HDRS, and
#   CORBA_SRV_ALL_HDRS

vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

HELLOPROCESS_NAME=HelloProcess
HELLOKILLER_NAME=HelloProcessKiller
SUPERVISION_OBSERVER_NAME=Supervision_Observer


# CDMW Generated Code
CDMW_GEN_HELLOPROCESS_SRC=
CDMW_GEN_SERVER_SRC=

# User Supplied Code
HELLOPROCESS_SRCS= $(CDMW_GEN_HELLOPROCESS_SRC) \
	HelloProcess.cpp

HELLOKILLER_SRCS= $(CDMW_GEN_HELLOPROCESS_SRC) \
	HelloProcessKiller.cpp

SUPERVISION_OBSERVER_SRCS=$(CDMW_GEN_SERVER_SRC) \
	SupervisionObserverPerf.cpp SupervisionObserverPerf_impl.cpp
	
.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


HELLOPROCESS_OBJ = $(HELLOPROCESS_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
HELLOKILLER_OBJ = $(HELLOKILLER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
SUPERVISION_OBSERVER_OBJ = $(SUPERVISION_OBSERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(HELLOKILLER_SRCS) $(HELLOPROCESS_SRCS) $(SUPERVISION_OBSERVER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend $(SUPERVISION_OBSERVER_NAME) $(HELLOPROCESS_NAME) $(HELLOKILLER_NAME)


.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(SUPERVISION_OBSERVER_NAME)
.PHONY: $(HELLOPROCESS_NAME)
.PHONY: $(HELLOKILLER_NAME)

.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(SUPERVISION_OBSERVER_NAME): $(SUPERVISION_OBSERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(SUPERVISION_OBSERVER_OBJ)  $(ALL_LIBS) $(CTOOLS_LIBS)

$(HELLOPROCESS_NAME): $(HELLOPROCESS_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(HELLOPROCESS_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)

$(HELLOKILLER_NAME): $(HELLOKILLER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(HELLOKILLER_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)

-include $(DEPENDS)
#Generated rules use the following variables:
# CXX,
# CPPFLAGS,
# CXXFLAGS,
# IDL2CXX,
# IDLFLAGS, and
# IDLMKINCLUDE

clean::
	-$(RM) *.o core
	-$(RM) $(CORBA_CL_ALL_SRC) $(CORBA_CL_ALL_HDRS)
	-$(RM) $(CORBA_SRV_ALL_SRC) $(CORBA_SRV_ALL_HDRS)
	-$(RM) $(DEPENDS)

cleanall: clean
	-$(RM) $(SUPERVISION_OBSERVER_NAME) $(HELLOPROCESS_NAME) $(HELLOKILLER_NAME)

# -----------------------------------------------

