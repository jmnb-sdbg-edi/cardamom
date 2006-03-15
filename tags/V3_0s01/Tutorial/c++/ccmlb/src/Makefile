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

include ../config.mk

# Export all variables by default to sub-makes
#export


MAINMK=	Main.mk

#IDL_SRCS= ../idl/ccm_ft.idl \

IDL_SRCS= ../generated/Cdmw_ccm_lb_cif.idl \
	../generated/ccm_lb_cif.idl \
	../idl/User_ccm_lb.idl	


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

all:  idl_var.mk idl_rules.mk
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@

clean cleanall:  idl_var.mk idl_rules.mk
	@echo "Executing $(MAINMK)"
	@${MAKE} -f $(MAINMK) $@
	-$(RM) idl_var.mk idl_rules.mk

