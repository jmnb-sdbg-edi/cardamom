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
# NOTA: The variables below includes CARDAMOM/ORB/OS specific variables
#   CPPFLAGS
#   CXXFLAGS
#   LDFLAGS
#   IDLFLAGS
#   LIBS

DEPEND_LIB=$(CSC_DEPEND_LIB)

include ../config.mk

#Append to CPPFLAGS and IDLFLAGS
override CPPFLAGS := -I. -I../include -I../generated \
	$(foreach d, $(shell find $(CDMW_HOME)/include/c++ -type d), -I$(d)) \
	$(CPPFLAGS)
override CPPFLAGS += -I../../../../cTools/src
override IDLFLAGS += -I../idl -I../generated

ALL_LIBS=$(LIBS) $(LDFLAGS_CSC) 

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

CCMSERVER_NAME=component_server
CLIENT_NAME=non_ccm_client
LIB_CCMSERVER_NAME=libServerCcmPerf.so


# CARDAMOM Generated Code

CDMW_GEN_CCMSERVER_COMMON_SRC=
CDMW_GEN_CCMSERVER_SRC=
CDMW_GEN_LIB_CCMSERVER_SRC=



# User Supplied Code
CCMSERVERS_COMMON_SRC=



CCMSERVER_SRCS=$(CCMSERVERS_COMMON_SRC) \
	$(CDMW_GEN_CCMSERVER_SRC) \
	$(CDMW_GEN_CCMSERVER_COMMON_SRC) \
	main_componentserver.cpp ServerCdmwInterface.cpp \
	ServerProcessControl.cpp \
	CCM_Server_SessionContext_impl.cpp \
	SessionServer_impl.cpp \
	SessionServerHome_impl.cpp \
	TestPing_impl.cpp

CLIENT_SRCS= ClientProcessControl.cpp client.cpp ClientCdmwInterface.cpp ClockTime.cpp

LIB_CCMSERVER_SRCS=$(CCMSERVERS_COMMON_SRC) \
	$(CDMW_GEN_CCMSERVER_SRC) \
	$(CDMW_GEN_CCMSERVER_COMMON_SRC) \
	TestPingFacetExecutor_impl.cpp \
	ServerHomeExecutor_impl.cpp \
	ServerExecutor_impl.cpp







.SUFFIXES:
+.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h



CCMSERVER_OBJ = $(CCMSERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o) $(CORBA_CL_ALL_OBJS)
#CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o)
LIB_CCMSERVER_OBJ = $(LIB_CCMSERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)



# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$ $(CCMSERVER_SRCS) $(CLIENT_SRCS) $(LIB_CCMSERVER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)

.PHONY: all
all: depend  $(LIB_CCMSERVER_NAME)  $(CCMSERVER_NAME) $(CLIENT_NAME) $(PACKAGING)
	@echo "Create CcmPerf package"
	../run/make_assembly_package.sh

.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(CCMSERVER_NAME)
.PHONY: $(CLIENT_NAME)
.PHONY: $(LIB_CCMSERVER_NAME)



#.cpp.o:
%.o:%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<



$(CCMSERVER_NAME): $(CCMSERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(CCMSERVER_OBJ)  $(ALL_LIBS) $(CTOOLS_LIBS)

$(CLIENT_NAME): $(CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -lxerces-c1_3 $(LDFLAGS) -o $@ \
	$(CLIENT_OBJ)  $(ALL_LIBS) $(CTOOLS_LIBS)

$(LIB_CCMSERVER_NAME): $(LIB_CCMSERVER_OBJ)
	@echo 
	$(PURIFY) $(CXXLD) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ \
	$(LIB_CCMSERVER_OBJ) $(ALL_LIBS) $(CTOOLS_LIBS)



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
	-$(RM) $(CCMSERVER_NAME) $(LIB_CCMSERVER_NAME)

# -----------------------------------------------


