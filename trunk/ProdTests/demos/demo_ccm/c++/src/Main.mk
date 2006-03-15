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
include idl_var.mk


vpath %.idl ../idl:../generated
vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

#CLIENT_NAME=client
SERVER1_NAME=component_server1
SERVER2_NAME=component_server2
LIB_CLIENT_NAME=libClientBasicDemo.so
LIB_SERVER_NAME=libServerBasicDemo.so


# CDMW Generated Code

CDMW_GEN_CLIENT_COMMON_SRC=
CDMW_GEN_CLIENT_SRC=
CDMW_GEN_LIB_CLIENT_SRC=


CDMW_GEN_SERVER_COMMON_SRC=
CDMW_GEN_SERVER_SRC=
CDMW_GEN_LIB_SERVER_SRC=



# User Supplied Code
SERVERS_COMMON_SRC=

LIB_CLIENT_SRCS=$(CDMW_GEN_LIB_CLIENT_SRC) \
	$(CDMW_GEN_CLIENT_COMMON_SRC) \
	CCM_ClientHome_impl.cpp \
	CCM_Client_impl.cpp


SERVER1_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER_SRC) \
	$(CDMW_GEN_SERVER_COMMON_SRC) \
	main_componentserver1.cpp CdmwInterface1.cpp \
	CCM_Client_SessionContext_impl.cpp Display_impl.cpp \
	SessionClient_impl.cpp CCM_Server_SessionContext_impl.cpp SessionServer_impl.cpp \
	SessionClientHome_impl.cpp  SessionServerHome_impl.cpp

SERVER2_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER_SRC) \
	$(CDMW_GEN_SERVER_COMMON_SRC) \
	main_componentserver2.cpp CdmwInterface2.cpp \
	CCM_Client_SessionContext_impl.cpp \
	SessionClient_impl.cpp \
	SessionClientHome_impl.cpp

LIB_SERVER_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER_SRC) \
	$(CDMW_GEN_SERVER_COMMON_SRC) \
	CCM_Display_impl.cpp CCM_ServerHome_impl.cpp CCM_Server_impl.cpp






.SUFFIXES:
+.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h



LIB_CLIENT_OBJ = $(LIB_CLIENT_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
SERVER1_OBJ = $(SERVER1_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
SERVER2_OBJ = $(SERVER2_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
LIB_SERVER_OBJ = $(LIB_SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)



# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$ $(LIB_CLIENT_SRCS) $(SERVER1_SRCS) $(SERVER2_SRCS) $(LIB_SERVER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)

.PHONY: all
all: depend  $(LIB_SERVER_NAME)  $(LIB_CLIENT_NAME) $(SERVER2_NAME) $(SERVER1_NAME) $(PACKAGING)
	@echo "Create BasicDemo package"
	$(CDMW_DEMO_HOME)/run/make_assembly_package.sh

.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(SERVER1_NAME)
.PHONY: $(SERVER2_NAME)
.PHONY: $(LIB_SERVER_NAME)
.PHONY: $(LIB_CLIENT_NAME)



#.cpp.o:
%.o:%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



$(SERVER1_NAME): $(SERVER1_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(SERVER1_OBJ)  $(ALL_LIBS)

$(SERVER2_NAME): $(SERVER2_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(SERVER2_OBJ)  $(ALL_LIBS)

$(LIB_SERVER_NAME): $(LIB_SERVER_OBJ)
	@echo 
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_SERVER_OBJ) $(ALL_LIBS)

$(LIB_CLIENT_NAME): $(LIB_CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_CLIENT_OBJ) $(ALL_LIBS)



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
	-$(RM) $(SERVER_NAME) $(LIB_SERVER_NAME) $(CLIENT_NAME) $(LIB_CLIENT_NAME)

# -----------------------------------------------

