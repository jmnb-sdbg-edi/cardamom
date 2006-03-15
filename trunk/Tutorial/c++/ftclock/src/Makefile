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

# Export all variables by default to sub-makes
#export


MAINMK=	Main.mk

IDL_SRCS= 


all client clientclock server:  
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@

clean cleanall:  
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@
	-$(RM) idl_var.mk idl_rules.mk
