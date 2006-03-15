#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved.
#* Software commonly developed by THALES and AMS.
#*
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
#* =========================================================================== *

SHELL = /bin/sh

include ../../site.mk
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
include ../../config.mk

#Append to CPPFLAGS and IDLFLAGS
override CPPFLAGS += -I. -I../include -I../generated -I$(XERCES_INC_PATH)/include -I$(TAO_ROOT)/tao \
        $(foreach d, $(shell find $(CDMW_HOME)/include/c++ -type d), -I$(d))
override IDLFLAGS += -I../idl -I../generated

#ALL_LIBS=$(LIBS)
ALL_LIBS=$(LIBS) $(LDFLAGS_CSC) -rdynamic -g -O2 -fPIC -L/tools/exec/cppunit-1.10.2/lib -lpthread\
       -lcdmwclockservice -ldl -lACE -lTAO -lTAO_PortableServer\
         -lTAO_CosNaming -lTAO_Messaging -lTAO_RTCORBA -lcdmwcommon -lcdmworbsupport -lcdmwossupport\
         -lcdmwcommonsvcsdatastore -lcdmwcommonsvcsfederation -lcdmwcommonsvcsnaming \
         -lcdmwcommonidl -lTAO_CosProperty -lTAO_IORTable -luuid\
         -lcppunit -lxerces-c1_3 -lcdmwossupport \
         -lcdmwftclockservice -lcdmwmonitoringidl\
         -lcdmwftcommon -lcdmwftstatemanager -lcdmwftinit -lcdmwftlocationmanager -lcdmwfaulttoleranceidl \
         -lTAO_PortableGroup -lTAO_DynamicAny -lTAO_FaultTolerance -lTAO_CosNotification -lTAO_DynamicInterface
         
#/tools/exec/TAO-OpenFusion-050826/src/ace/libTAO_PortableGroup.so.1.4.1@
#/tools/exec/TAO-OpenFusion-050826/src/ace/libTAO_PortableServer.so@
#/tools/exec/TAO-OpenFusion-050826/src/ace/libTAO_PortableServer.so.1.4.1@
#/tools/exec/TAO-OpenFusion-050826/src/ace/libTAO_RTPortableServer.so@
#/tools/exec/TAO-OpenFusion-050826/src/ace/libTAO_RTPortableServer.so.1.4.1@



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

CLIENT_NAME=client
CLIENT_CLOCK_NAME=clientclock



# CDMW Generated Code
CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.cpp

# User Supplied Code
CLIENT_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	Client.cpp ClientProcessBehaviour.cpp ClientThread.cpp TimeoutHandler.cpp
CLIENT_CLOCK_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	ClientClock.cpp ClientClockProcessBehaviour.cpp ClientClockThread.cpp TimeoutHandler.cpp ClientProcessBehaviour.cpp ClientThread.cpp


.SUFFIXES:
.SUFFIXES: .idl .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o) $(CORBA_CL_ALL_OBJS)
CLIENT_CLOCK_OBJ = $(CLIENT_CLOCK_SRCS:.cpp=.o) $(CORBA_CL_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(CLIENT_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend  $(CLIENT_NAME) $(CLIENT_CLOCK_NAME)


.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(CLIENT_NAME) $(CLIENT_CLOCK_NAME)



.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(CLIENT_NAME): $(CLIENT_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(CLIENT_OBJ) $(ALL_LIBS)
    
$(CLIENT_CLOCK_NAME): $(CLIENT_CLOCK_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(CLIENT_CLOCK_OBJ) $(ALL_LIBS)

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
	-$(RM) $(CLIENT_NAME)
	-$(RM) $(CLIENT_CLOCK_NAME)

# -----------------------------------------------

