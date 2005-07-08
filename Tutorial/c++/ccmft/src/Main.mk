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

include ../../site.mk
# Imported variables

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

SERVER_NAME=server_comp_srv
CLIENT_NAME=client_comp_srv
GRP_CREATOR_NAME=group_creator
#DATASTORE_NAME=datastore
LIB_CLIENT_NAME=libcdmwValidCcmFtClient.so
LIB_SERVER_NAME=libcdmwValidCcmFtServer.so


# CDMW Generated Code
#CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.cpp
#CDMW_GEN_SERVER_SRC=ServerCdmwInterface.cpp
#CDMW_GEN_GRP_CREATOR_SRC=GroupCreatorCdmwInterface.cpp 

CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.cpp
CDMW_GEN_GRP_CREATOR_SRC=GroupCreatorCdmwInterface.cpp
CDMW_GEN_SERVER_SRC=ServerCdmwInterface.cpp



# User Supplied Code
CLIENT_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	CCM_Client_SessionContext_impl.cpp SessionClientHome_impl.cpp \
	SessionClient_impl.cpp \
	HostnameResultConsumer_impl.cpp \
	HostInfo_impl.cpp \
	client.cpp 

SERVER_SRCS=$(CDMW_GEN_SERVER_SRC) \
	CCM_ServerWithEvents_SessionContext_impl.cpp \
	CCM_ServerWithFacet_SessionContext_impl.cpp \
	SessionServerWithEventsHome_impl.cpp \
	SessionServerWithFacetHome_impl.cpp \
	SessionServerWithFacet_impl.cpp \
	SessionServerWithEvents_impl.cpp \
	HostInfo_impl.cpp \
	CommandConsumer_impl.cpp \
        server.cpp

LIB_SERVER_SRCS= CCM_ServerWithFacet_impl.cpp \
        CCM_ServerWithFacetHome_impl.cpp \
        CCM_ServerWithEvents_impl.cpp \
        CCM_ServerWithEventsHome_impl.cpp \
        Command_impl.cpp \
        HostnameResult_impl.cpp


LIB_CLIENT_SRCS= CCM_Client_impl.cpp \
        CCM_ClientHome_impl.cpp \
        Command_impl.cpp \
        HostnameResult_impl.cpp


GRP_CREATOR_SRCS=$(CDMW_GEN_GRP_CREATOR_SRC) \
	GroupCreatorProcessBehaviour.cpp \
	group_creator.cpp


.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


SERVER_OBJ = $(SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
LIB_SERVER_OBJ = $(LIB_SERVER_SRCS:.cpp=.o) 
#$(CORBA_CL_ALL_OBJS)
LIB_CLIENT_OBJ = $(LIB_CLIENT_SRCS:.cpp=.o) 
#$(CORBA_CL_ALL_OBJS)
GRP_CREATOR_OBJ = $(GRP_CREATOR_SRCS:.cpp=.o) $(CORBA_CL_ALL_OBJS)


# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(CLIENT_SRCS) $(SERVER_SRCS) $(LIB_CLIENT_SRCS)  $(LIB_SERVER_SRCS) $(GRP_CREATOR_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend $(SERVER_NAME) $(CLIENT_NAME) $(LIB_CLIENT_NAME) $(LIB_SERVER_NAME) $(GRP_CREATOR_NAME)
	@echo "Create Database package"
	../run/make_assembly_package.sh

.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(SERVER_NAME)
.PHONY: $(CLIENT_NAME)
.PHONY: $(LIB_CLIENT_NAME) 
.PHONY: $(LIB_SERVER_NAME)
.PHONY: $(GRP_CREATOR_NAME)



.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<




$(SERVER_NAME): $(SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(SERVER_OBJ)  $(ALL_LIBS)

$(CLIENT_NAME): $(CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(CLIENT_OBJ) $(ALL_LIBS)

$(LIB_CLIENT_NAME): $(LIB_CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_CLIENT_OBJ)  $(ALL_LIBS)

$(LIB_SERVER_NAME): $(LIB_SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_SERVER_OBJ)  $(ALL_LIBS)

$(GRP_CREATOR_NAME): $(GRP_CREATOR_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(GRP_CREATOR_OBJ)  $(ALL_LIBS)

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
	-$(RM) $(SERVER_NAME) $(CLIENT_NAME) $(LIB_CLIENT_NAME) $(LIB_SERVER_NAME) $(GRP_CREATOR_NAME)

# -----------------------------------------------

