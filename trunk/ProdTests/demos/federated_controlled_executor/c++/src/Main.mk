#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#* 
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
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
	$(foreach d, $(shell find $(CDMW_HOME)/include/c++ -name TAO\* -prune -o -type d -print), -I$(d)) \
        -I$(CDMW_HOME)/include/c++/TAOOpenFusion
override IDLFLAGS += -I../idl -I../generated
ALL_LIBS=$(LIBS)

# Exported variables
#   CORBA_ALL_IDL,
#   CORBA_CL_ALL_SRC,
#   CORBA_SRV_ALL_SRC,
#   CORBA_CL_ALL_OBJS,
#   CORBA_SRV_ALL_OBJS,
#   CORBA_CL_ALL_HDRS, and
#   CORBA_SRV_ALL_HDRS
#MAC COMMENT: file idl_var.mk should not exist
#include idl_var.mk

vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

TIMEOUT_HANDLER=TimeoutHandler
TIME_READER=TimeReader
TIME_ADMIN=TimeAdmin
GET_CURRENT_TIME=GetCurrentTime

# User Supplied Code
TIMEOUT_HANDLER_SRCS=TimeoutHandler.cpp
TIME_READER_SRCS=TimeReader.cpp
TIME_ADMIN_SRCS=TimeAdmin.cpp
GET_CURRENT_TIME_SRCS=GetCurrentTime.cpp

.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h

TIMEOUT_HANDLER_OBJ = $(TIMEOUT_HANDLER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
TIME_READER_OBJ = $(TIME_READER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
TIME_ADMIN_OBJ = $(TIME_ADMIN_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
GET_CURRENT_TIME_OBJ = $(GET_CURRENT_TIME_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

SRCS=$(TIMEOUT_HANDLER_SRCS) $(TIME_READER_SRCS) $(TIME_ADMIN_SRCS) $(GET_CURRENT_TIME_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)

.PHONY: all
all: depend $(TIMEOUT_HANDLER) $(TIME_READER) $(TIME_ADMIN) $(GET_CURRENT_TIME)

.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(TIMEOUT_HANDLER)
.PHONY: $(TIME_READER)
.PHONY: $(TIME_ADMIN)

.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(TIMEOUT_HANDLER): $(TIMEOUT_HANDLER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(TIMEOUT_HANDLER_OBJ) $(ALL_LIBS)

$(TIME_READER): $(TIME_READER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(TIME_READER_OBJ) $(ALL_LIBS)

$(TIME_ADMIN): $(TIME_ADMIN_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(TIME_ADMIN_OBJ) $(ALL_LIBS)

$(GET_CURRENT_TIME): $(GET_CURRENT_TIME_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(GET_CURRENT_TIME_OBJ) $(ALL_LIBS)

-include $(DEPENDS)

#Generated rules use the following variables:
# CXX,
# CPPFLAGS,
# CXXFLAGS,
# IDL2CXX,
# IDLFLAGS, and
# IDLMKINCLUDE
#MAC COMMENT: file idl_rules.mk should not exist
#include idl_rules.mk

clean::
	-$(RM) *.o core
	-$(RM) $(CORBA_CL_ALL_SRC) $(CORBA_CL_ALL_HDRS)
	-$(RM) $(CORBA_SRV_ALL_SRC) $(CORBA_SRV_ALL_HDRS)
	-$(RM) $(DEPENDS)

cleanall: clean
	-$(RM) $(TIMEOUT_HANDLER) $(TIME_READER) $(TIME_ADMIN) $(GET_CURRENT_TIME)

# -----------------------------------------------
