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


#################################################################################
# DO NOT UPDATE BELOW!!!!!!
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
LDFLAGS_linux= -lpthread -ldl -rdynamic
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
	-DCDMW_TRACE_LEVEL=5 -D_REENTRANT -DCDMW_ASSERT_NO_THROW
CXXFLAGS_CDMW=
LDFLAGS_CDMW=-L$(CDMW_HOME)/lib/c++ \
	-L$(XERCES_LIB_PATH)
IDLFLAGS_CDMW=-I$(CDMW_HOME)/idl


LIBS_CDMW=-lcdmwcommon -lcdmworbsupport -lcdmwossupport -lcdmwosthreads \
	-lcdmwcommonsvcsnaming -lcdmwrepositoryinterface -lcdmwplatforminterface \
	-lcdmwcdmwinit -lcdmwlifecycle -lcdmwtestutils -lcdmwftinit \
	-lcdmwftlocationmanager  -lcdmwcommonsvcsdatastore -lcdmwftcommon \
	-lcdmwftstatemanager -lcdmwmonitoringidl -lcdmweventinterface\
	-lxerces-c1_3 -lcdmwlifecycleidl -lcdmweventsupport -lcdmweventidl \
	-lcdmwplatformvaluetypes -lcdmwplatformlibrary -lcdmwlifecycleinit \
	-lcdmwpullmonitorableidl -lcdmwsmgcommonidl -lcdmwsmginterfaceidl \
	-lcdmwfaulttoleranceidl -lcdmwrepositoryidl \
	-lcdmwsystemmngtidl -lcdmwcommonidl -lcdmwtraceidl -lcdmwtracelibrary \
	-lcdmwccmcommon -lcdmwccmcif -lcdmwccmcontainer -lcdmwccmcontaineridl \
	-lcdmwccmcomponentserver -lcdmwlogging



"LIBS_CDMW:=$(foreach CSC,$(DEPEND_LIB),-lcdmw$(CSC))
#LIBS_CDMW=-lcdmwcdmwinit -lcdmweventsupport -lcdmweventinterface -lcdmwlifecycle -lcdmwrepositoryinterface  \
#	-lcdmwplatforminterface -lcdmwtracelibrary -lcdmwperformancelibrary -lcdmwidl -lcdmwnaminginterface \
#	-lcdmworbsupport -lcdmwossupport -lcdmwosthreads -lcdmwcommon \
#	-lxerces-c1_3
#END CARDAMOM FLAGS

RM=rm -f
CXX=g++
CXXLD=g++

CPPFLAGS=$(CPPFLAGS_$(HOST_SUFFIX)) $(CPPFLAGS_ORB) $(CPPFLAGS_CDMW)
CXXFLAGS=$(CXXFLAGS_$(HOST_SUFFIX)) $(CXXFLAGS_ORB) $(CXXFLAGS_CDMW)
LDFLAGS=$(LDFLAGS_$(HOST_SUFFIX)) $(LDFLAGS_ORB) $(LDFLAGS_CDMW)
IDLFLAGS=$(IDLFLAGS_$(HOST_SUFFIX)) $(IDLFLAGS_CDMW) $(IDLFLAGS_ORB) 
LIBS=$(LIBS_$(HOST_SUFFIX)) $(LIBS_ORB) $(LIBS_CDMW)

