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

# Export all variables by default to sub-makes

MAINMK=Main.mk

#MAC COMMENT: macro IDL_SRCS should be empty
#IDL_SRCS= ../idl/ProcessAgent.idl \
	../idl/AnonymousResourceFactory.idl \
	../idl/ResourceFactory.idl \
	../idl/Echo.idl \
	../idl/CF.idl
IDL_SRC=

.PHONY: idl_var.mk
idl_var.mk: $(IDL_SRCS)
	@echo "Creating idl makefiles (variable)."
	@$(RM) $@
	@$(IDLMKDEPEND) --mk-include --orb $(ORB) \
	--variables-only --old-make \
	$(IDL_SRCS) > $@

.PHONY: idl_rules.mk
idl_rules.mk: $(IDL_SRCS)
	@echo "Creating idl makefiles (rules)."
	@$(RM) $@
	@$(IDLMKDEPEND) --mk-include --orb $(ORB) \
	--rules-only \
	$(IDL_SRCS) > $@

#MAC COMMENT: targets 'all', 'TimeoutHandler', 'TimeReader' and 'TimeAdmin' should not depend on files idl_var.mk and idl_rules.mk
#all TimeoutHandler TimeReader TimeAdmin: idl_var.mk idl_rules.mk
all TimeoutHandler TimeReader TimeAdmin GetCurrentTime:
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@

#MAC COMMENT: targets 'clean' and 'cleanall' should not depend on files idl_var.mk and idl_rules.mk
#clean cleanall: idl_var.mk idl_rules.mk
clean cleanall:
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@
	-$(RM) idl_var.mk idl_rules.mk

