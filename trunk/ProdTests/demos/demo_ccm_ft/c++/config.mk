#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved.
#* Software commonly developed by THALES and SELEX-SI
#*
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
#* =========================================================================== *

################################################################################
# DO NOT EDIT !!!!!!
################################################################################

# NOTA: This file requires prior inclusion of site.mk file
ifeq ($(strip $(CDMW_DEMO_HOME)),)
AbortMake:
	@echo "This file requires prior inclusion of site.mk file"
	@exit 1
endif

# We determine the compilation host
HOST_TYPE:=$(shell $(CDMW_HOME)/bin/config.guess)
HOST_SUFFIX:=$(shell echo $(HOST_TYPE) | cut -f 3 -d"-" | tr "." "_")


# ------------------------------------------------------------------------------
# Compilation flag for each supported target
# ------------------------------------------------------------------------------
# Solaris 2.7
# ...........
CXXFLAGS_solaris2_7=-g -Wall -Werror -fpic
CPPFLAGS_solaris2_7= -I$(STLPORT_INC_PATH)/stlport
LDFLAGS_solaris2_7= -L$(STLPORT_LIB_PATH) \
	-lpthread -lrt -lsocket -lnsl -lstlport_gcc_debug -lstlport_gcc_stldebug
IDLFLAGS_solaris2_7=
LIBS_solaris2_7=

# Solaris 2.8
# ...........
CXXFLAGS_solaris2_8=-g -Wall -Werror -fPIC
CPPFLAGS_solaris2_8= -I$(STLPORT_INC_PATH)/stlport  -nostdinc++
LDFLAGS_solaris2_8= -L$(STLPORT_LIB_PATH) \
	-lpthread -lrt -lsocket -lnsl -lstlport_gcc_debug -lstlport_gcc_stldebug
IDLFLAGS_solaris2_8=
LIBS_solaris2_8=


# Linux
# ...........
#CXXFLAGS_linux=-g -Wall -Werror
#CXXFLAGS_linux=-g -Wall -Werror -fPIC
CXXFLAGS_linux=-Wall
LDFLAGS_linux=  -lpthread -ldl -rdynamic
IDLFLAGS_linux=
LIBS_linux=
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Compilation flag for the ORB as needed by CDMW
# ------------------------------------------------------------------------------
include $(CDMW_DEMO_HOME)/$(ORB).mk

#Variables needed for jtk_idl_depend.sh and mk_idl_include.sh scripts
IDLMKDEPEND=$(CDMW_HOME)/bin/jtk_idl_depend.sh
IDLMKINCLUDE=$(CDMW_HOME)/bin/mk_idl_include.sh
IDL2CXX=$(IDL2CXX_$(ORB))

CPPFLAGS_ORB= $(CPPFLAGS_$(ORB))
CXXFLAGS_ORB= $(CXXFLAGS_$(ORB))
IDLFLAGS_ORB= $(IDLFLAGS_$(ORB))
LDFLAGS_ORB:= $(LDFLAGS_$(ORB))
LIBS_ORB:= $(LIBS_$(ORB))


#BEGIN CDMW FLAGS
CPPFLAGS_CDMW=-I$(CDMW_HOME)/include/c++ \
	-I$(CDMW_HOME)/include/c++/cdmwidl \
	-I$(XERCES_INC_PATH) \
	-I$(SPLICE_INC_PATH)/C++/CCPP/OpenFusion -I$(SPLICE_INC_PATH)/C++/CCPP \
	-I$(TAO_ROOT)/include \
	-D_REENTRANT -DCDMW_ASSERT_NO_THROW -DDDS_OpenFusion_1_4_1
CXXFLAGS_CDMW=
LDFLAGS_CDMW=-L$(CDMW_HOME)/lib/c++ \
	-L$(XERCES_LIB_PATH) -L$(SPLICE_LIB_PATH) 
IDLFLAGS_CDMW=-I$(CDMW_HOME)/idl
LIBS_CDMW=-lcdmwcdmwinit -lcdmweventsupport -lcdmweventinterface \
          -lcdmwlifecycle -lcdmwrepositoryinterface \
          -lcdmwplatforminterface -lcdmwcommonsvcsnaming \
          -lcdmworbsupport -lcdmwossupport -lcdmwosthreads -lcdmwlogging \
          -lcdmwcommon -lcdmwccmcommon -lcdmwccmcif -lcdmwccmcontainer \
          -lcdmwccmcomponentserver -lcdmwtestutils -lxerces-c1_3 \
          -lcdmwlifecycleidl -lcdmweventidl -lcdmwccmcontaineridl \
          -lcdmwcommonidl -lcdmwrepositoryidl \
          -lcdmwplatformvaluetypes -lcdmwplatformlibrary \
          -lcdmwpullmonitorableidl -lcdmwsmgcommonidl -lcdmwsmginterfaceidl \
          -lcdmwfaulttoleranceidl -lcdmwcosnotificationidl \
          -lcdmwftinit -lcdmwftlocationmanager -lcdmwcommonsvcsdatastore \
          -lcdmwftcommon -lcdmwftstatemanager -lcdmwmonitoringidl \
	  -ldcpscommon -ldcpsccpp -ldcpsgapi -lstdc++
#-ldcpscorbac++ 
#END CDMW FLAGS

RM=rm -f
CXX=g++
CXXLD=g++

CPPFLAGS=$(CPPFLAGS_$(HOST_SUFFIX)) $(CPPFLAGS_ORB) $(CPPFLAGS_CDMW)
CXXFLAGS=$(CXXFLAGS_$(HOST_SUFFIX)) $(CXXFLAGS_ORB) $(CXXFLAGS_CDMW)
LDFLAGS=$(LDFLAGS_$(HOST_SUFFIX)) $(LDFLAGS_ORB) $(LDFLAGS_CDMW)
IDLFLAGS=$(IDLFLAGS_$(HOST_SUFFIX)) $(IDLFLAGS_CDMW) $(IDLFLAGS_ORB) 
LIBS=$(LIBS_$(HOST_SUFFIX))$(LIBS_CDMW) $(LIBS_ORB)

