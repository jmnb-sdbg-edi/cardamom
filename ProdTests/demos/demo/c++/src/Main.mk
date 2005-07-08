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
	$(foreach d, $(shell find $(CDMW_HOME)/include/c++ -type d), -I$(d))
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
include idl_var.mk


vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

CLIENT_NAME=client
SERVER1_NAME=server1
SERVER2_NAME=server2


# CDMW Generated Code
CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.cpp
CDMW_GEN_SERVERS_COMMON_SRC=ResourceServantProvider.cpp
CDMW_GEN_SERVER1_SRC=SessionResourceFactory_impl.cpp \
		Server1CdmwInterface.cpp \
		EntityResourceFactory_impl.cpp
CDMW_GEN_SERVER2_SRC=Server2CdmwInterface.cpp \
		ServiceResourceFactory_impl.cpp \
		UnMngEntityResourceFactory_impl.cpp

# User Supplied Code
SERVERS_COMMON_SRC=Application_impl.cpp \
		Device_impl.cpp \
		ProcessAgentFactory_impl.cpp \
		UserResourceServantProvider.cpp \
	 	Echo_impl.cpp ServerProcessControl.cpp

CLIENT_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	ClientThread.cpp \
	MyProcessControl.cpp \
	TestSessionStrategy.cpp \
	TestServiceStrategy.cpp \
	TestUnManagedEntityStrategy.cpp \
	TestUnManagedEntityStrategyRestart.cpp \
	TestEntityStrategy.cpp \
	TestEntityStrategyRestart.cpp \
	TestData.cpp \
	TestEventService.cpp \
	client.cpp

SERVER1_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER1_SRC) \
	$(CDMW_GEN_SERVERS_COMMON_SRC) \
	server1.cpp

SERVER2_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER2_SRC) \
	$(CDMW_GEN_SERVERS_COMMON_SRC) \
	UserAnonymousResourceServantProvider.cpp \
	server2.cpp


.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
SERVER1_OBJ = $(SERVER1_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
SERVER2_OBJ = $(SERVER2_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(CLIENT_SRCS) $(SERVER1_SRCS) $(SERVER2_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend $(SERVER1_NAME) $(SERVER2_NAME) $(CLIENT_NAME) 


.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(SERVER1_NAME)
.PHONY: $(SERVER2_NAME)
.PHONY: $(CLIENT_NAME)


.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(SERVER1_NAME): $(SERVER1_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(SERVER1_OBJ)  $(ALL_LIBS)

$(SERVER2_NAME): $(SERVER2_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(SERVER2_OBJ) $(ALL_LIBS)

$(CLIENT_NAME): $(CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(CLIENT_OBJ) $(ALL_LIBS)

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
	-$(RM) $(SERVER1_NAME) $(SERVER2_NAME) $(CLIENT_NAME)

# -----------------------------------------------
