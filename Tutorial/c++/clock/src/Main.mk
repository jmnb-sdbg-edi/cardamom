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
# NOTA: The variables below includes CARDAMOM/ORB/OS specific variables
#   CPPFLAGS
#   CXXFLAGS
#   LDFLAGS
#   IDLFLAGS
#   LIBS

include ../src/CSC.mk

DEPEND_LIB=$(CSC_DEPEND_LIB)

include ../../config.mk


#Append to CPPFLAGS and IDLFLAGS


override CPPFLAGS := -g -Dlinux \
	    -D_REENTRANT \
	    -DCDMW_ASSERT_NO_THROW \
	    -DCDMW_POSIX \
	    -DCDMW_ORB_VDR=4203 \
	    -DCDMW_ORB_VER=14 \
	    -D_POSIX_THREADS \
	    -D_POSIX_THREAD_SAFE_FUNCTIONS \
	    -DACE_HAS_EXCEPTIONS \
	    -DACE_HAS_AIO_CALLS \
	    -DTAO_HAS_VALUETYPE \
	    -DTAO_USE_SEQUENCE_TEMPLATES \
	    -Dtao=4203 \
	    -I$(ACE_ROOT) \
	    -I$(TAO_ROOT) \
	    -I$(TAO_ROOT)/tao \
	    -I$(TAO_ROOT)/tao/IFR_Client \
	    -I$(TAO_ROOT)/orbsvcs \
	    -I$(TAO_ROOT)/orbsvcs/orbsvcs/IFRService \
	    -I/tools/exec/cppunit/include \
	    -I$(CDMW_HOME)/include/c++ \
        $(foreach d, $(shell find $(CDMW_HOME)/include/c++ -type d), -I$(d)) \
	    -O0 -g3 -Wall -Wno-deprecated \
	    -rdynamic -fPIC

override IDLFLAGS += -I -I../generated

ALL_LIBS=-L$(CDMW_HOME)/lib/c++ \
	    -lcdmwlogging -lcdmwclockservice \
	    -lcdmwcommonidl \
	    -lcdmwcommon \
	    -lcdmwcommonsvcsdatastore \
	    -lcdmwcommonsvcsfederation \
	    -lcdmwcommonsvcsnaming \
	    -lcdmworbsupport \
	    -lcdmwossupport \
	    -lcdmwosthreads \
	    -lcdmwtestutils \
	    -L/tools/exec/cppunit-1.10.2/lib \
	    -lcppunit \
	    -L/tools/exec/xerces-c-src1_3_0/lib \
	    -lxerces-c1_3 \
	    -L/tools/exec/TAO-OpenFusion-050826/src/ace \
	    -lACE \
	    -lTAO \
	    -lTAO_PortableServer \
	    -lTAO_CosNaming \
	    -lTAO_Messaging \
	    -lTAO_CosProperty \
	    -lTAO_IORTable \
	    -lpthread -ldl -luuid

# Exported variables
#   CORBA_ALL_IDL,
#   CORBA_CL_ALL_SRC,
#   CORBA_SRV_ALL_SRC,
#   CORBA_CL_ALL_OBJS,
#   CORBA_SRV_ALL_OBJS,
#   CORBA_CL_ALL_HDRS, and
#   CORBA_SRV_ALL_HDRS

vpath %.cpp ../src:../generated
vpath %.hpp ../include:../generated

CLIENT_NAME=clock
SERVER_NAME=


# User Supplied Code

CLIENT_SRCS=Clock.cpp

SERVER_SRCS=


.SUFFIXES:
.SUFFIXES: .cpp .skel.cpp .stub.cpp .o .stub.hpp .skel.hpp .hpp .h


CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o) $(CORBA_CL_ALL_OBJS)
SERVER_OBJ = $(SERVER_SRCS:.cpp=.o) $(CORBA_SRV_ALL_OBJS)

# Rules to build the dependency of each source file
%.d: %.cpp
	@echo "Building dependency of $<"
	@$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


SRCS=$(CLIENT_SRCS) $(SERVER_SRCS) $(CORBA_SRV_ALL_SRC)
DEPENDS=$(SRCS:.cpp=.d)



.PHONY: all
all: depend $(SERVER_NAME) $(CLIENT_NAME)


.PHONY: depend
depend: $(DEPENDS)

.PHONY: $(SERVER_NAME)
.PHONY: $(CLIENT_NAME)


.cpp.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(SERVER_NAME): $(SERVER_OBJ)
	$(PURIFY) $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	$(SERVER_OBJ)  $(ALL_LIBS)

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

clean::
	-$(RM) *.o core
	-$(RM) $(CORBA_CL_ALL_SRC) $(CORBA_CL_ALL_HDRS)
	-$(RM) $(CORBA_SRV_ALL_SRC) $(CORBA_SRV_ALL_HDRS)
	-$(RM) $(DEPENDS)

cleanall: clean
	-$(RM) $(SERVER_NAME) $(CLIENT_NAME)

# -----------------------------------------------


