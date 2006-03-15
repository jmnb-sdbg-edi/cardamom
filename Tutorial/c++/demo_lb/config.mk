#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
#* =========================================================================== *


#################################################################################
# DO NOT EDIT !!!!!!
#################################################################################

# NOTA: This file requires prior inclusion of site.mk file
ifeq ($(strip $(CDMW_TUTORIAL_HOME)),)
AbortMake:
	@echo "This file requires prior inclusion of site.mk file"
	@exit 1
endif

# We determine the compilation host
HOST_TYPE:=$(shell $(CDMW_HOME)/bin/config.guess)
HOST_SUFFIX:=$(shell echo $(HOST_TYPE) | cut -f 3 -d"-" | tr "." "_")


# -------------------------------------------------------------------------------
# Compilation flag for each supported target
# -------------------------------------------------------------------------------
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
LDFLAGS_solaris2_8= -Wl,-G -nostdinc++ -L$(STLPORT_LIB_PATH) \
	-lpthread -lrt -lsocket -lnsl -lstlport_gcc_debug -lstlport_gcc_stldebug
IDLFLAGS_solaris2_8=
LIBS_solaris2_8=


# Linux
# ...........
CXXFLAGS_linux=-g -Wall -Werror -fPIC
#CPPFLAGS_linux= -I$(CDMW_HOME)/include/c++/deficiency/gcc-2-95-2
LDFLAGS_linux= -lpthread -ldl -rdynamic -luuid
IDLFLAGS_linux=
LIBS_linux=
# -------------------------------------------------------------------------------

# -------------------------------------------------------------------------------
# Compilation flag for the ORB as needed by CARDAMOM
# -------------------------------------------------------------------------------
include $(CDMW_TUTORIAL_HOME)/$(ORB).mk

#Variables needed for jtk_idl_depend.sh and mk_idl_include.sh scripts
IDLMKDEPEND=$(CDMW_HOME)/bin/jtk_idl_depend.sh
IDLMKINCLUDE=$(CDMW_HOME)/bin/mk_idl_include.sh
IDL2CXX=$(IDL2CXX_$(ORB))

CPPFLAGS_ORB= $(CPPFLAGS_$(ORB))
CXXFLAGS_ORB= $(CXXFLAGS_$(ORB))
IDLFLAGS_ORB= $(IDLFLAGS_$(ORB))
LDFLAGS_ORB:= $(LDFLAGS_$(ORB))
LIBS_ORB:= $(LIBS_$(ORB))


#BEGIN CARDAMOM FLAGS
CPPFLAGS_CDMW=-I$(CDMW_HOME)/include/c++ \
	-I$(XERCES_INC_PATH) \
	-D_REENTRANT -DCDMW_ASSERT_NO_THROW
CXXFLAGS_CDMW=
LDFLAGS_CDMW=-L$(CDMW_HOME)/lib/c++ \
	-L$(XERCES_LIB_PATH)
IDLFLAGS_CDMW=-I$(CDMW_HOME)/idl


LIBS_CDMW=-lcdmwcommon -lcdmwcommonidl -lcdmworbsupport -lcdmwossupport -lcdmwosthreads \
	-lcdmwcommonsvcsnaming -lcdmwrepositoryinterface -lcdmwlifecycle \
	-lcdmwcommonsvcsdatastore -lxerces-c1_3 -lcdmwlifecycleidl \
	-lcdmwplatformvaluetypes -lcdmwplatformlibrary \
	-lcdmwpullmonitorableidl -lcdmwsmgcommonidl -lcdmwsmginterfaceidl \
	-lcdmwrepositoryidl  -lcdmwgroupmanageridl \
	-lcdmwcdmwinit -lcdmwplatforminterface \
	-lcdmwcommonidl -lcdmwtraceidl -lcdmwtracelibrary -lcdmwcommonsvcsfederation \
	-lcdmwlogging -lcdmwlbinit -lcdmwlbgroupmanager -lcdmwlbstrategy -lcdmwlbcommon -lcdmwloadbalancingidl \
	-lTAO_DynamicInterface

RM=rm -f
CXX=g++
CXXLD=g++

CPPFLAGS=$(CPPFLAGS_$(HOST_SUFFIX)) $(CPPFLAGS_ORB) $(CPPFLAGS_CDMW)
CXXFLAGS=$(CXXFLAGS_$(HOST_SUFFIX)) $(CXXFLAGS_ORB) $(CXXFLAGS_CDMW)
LDFLAGS=$(LDFLAGS_$(HOST_SUFFIX)) $(LDFLAGS_ORB) $(LDFLAGS_CDMW)
IDLFLAGS=$(IDLFLAGS_$(HOST_SUFFIX)) $(IDLFLAGS_CDMW) $(IDLFLAGS_ORB) 
LIBS=$(LIBS_$(HOST_SUFFIX)) $(LIBS_ORB) $(LIBS_CDMW)

