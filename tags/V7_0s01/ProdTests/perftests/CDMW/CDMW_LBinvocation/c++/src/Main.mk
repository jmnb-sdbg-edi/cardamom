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

################################################################################
#* SELEX-SI - CARDAMOM
#*
#* Copyright (c) SELEX-SI 2004-2005 All rights reserved.
#* Software commonly developed by SELEX-SI and THALES.
#*                                               
#* This  file and the  information  it contains  are  property  of   SELEX-SI  and
#* confidential.  They  shall not be reproduced nor disclosed  to any person,
#* except to those having a need  to know them, without prior written consent *
#  of SELEX-SI.
################################################################################

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
override CPPFLAGS += -I../../../../cTools/src
override IDLFLAGS += -I../idl -I../generated

ALL_LIBS=$(LIBS)

CTOOLS_LIBS=-L../../../../cTools/lib -lCTools

# Exported variables
#   CORBA_ALL_IDL,
#   CORBA_CL_ALL_SRC,
#   CORBA_SRV_ALL_SRC,
#   CORBA_CL_ALL_OBJS,
#   CORBA_SRV_ALL_OBJS,
#   CORBA_CL_ALL_HDRS, and
#   CORBA_SRV_ALL_HDRS
include idl_var.mk


vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

REPLICA_NAME=hello_server
PING_NAME=client
BINDER_NAME=bind_replicas

# CDMW Generated Code
#CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.cpp
#CDMW_GEN_SERVER_SRC=ServerCdmwInterface.cpp

# User Supplied Code
REPLICA_SRCS= \
      TestUtils.cpp \
      CORBAManager.cpp \
      TestPingable_impl.cpp \
      mainServer.cpp \

PING_SRCS= \
      TestUtils.cpp \
      CORBAManager.cpp \
      mainPing.cpp \
 
BINDER_SRCS= \
      mainBind.cpp \
      TestUtils.cpp \
      CORBAManager.cpp \

.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


REPLICA_OBJ = $(REPLICA_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
PING_OBJ = $(PING_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
BINDER_OBJ = $(BINDER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(REPLICA_SRCS) $(PING_SRCS) $(BINDER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend $(REPLICA_NAME) $(PING_NAME) $(BINDER_NAME)


.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(REPLICA_NAME)
.PHONY: $(PING_NAME)
.PHONY: $(BINDER_NAME)

.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(REPLICA_NAME): $(REPLICA_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(REPLICA_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)

$(PING_NAME): $(PING_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(PING_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)

$(BINDER_NAME): $(BINDER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(BINDER_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)

-include $(DEPENDS)
#Generated rules use the following variables:
# CXX,
# CPPFLAGS,
# CXXFLAGS,
# IDL2CXX,
# IDLFLAGS, and
# IDLMKINCLUDE
include idl_rules.mk

clean::
	-$(RM) *.o core
	-$(RM) $(CORBA_CL_ALL_SRC) $(CORBA_CL_ALL_HDRS)
	-$(RM) $(CORBA_SRV_ALL_SRC) $(CORBA_SRV_ALL_HDRS)
	-$(RM) $(DEPENDS)

cleanall: clean
	-$(RM) $(REPLICA_NAME) $(PING_NAME) $(BINDER_NAME)

# -----------------------------------------------

