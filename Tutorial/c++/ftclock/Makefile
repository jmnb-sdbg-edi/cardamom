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

PWD:=$(shell pwd)


# Imported variables
#   
#   CDMW_HOME
include ../site.mk

BUILD_TARGET:=$(shell $(CDMW_HOME)/bin/config.guess)

XML_CONF_FILES:=xml/Main.xml

XML_DATA_FILES:=data/Client.xml data/Server.xml data/GroupCreator.xml \
	data/Scenario.xml data/ScenarioTimers.xml data/Cdmw_nar.xml \
	data/CdmwPlatformMngtDaemon_conf.xml \
	data/FTClock.xml \
	data/CdmwFaultToleranceManager_clock_conf.xml  \
	data/CdmwFTSystemMngtGroupCreator_conf.xml \
	data/cdmw_nar_integ_clock.xml  \
	data/ClientClock.xml 

.SUFFIXES:
.SUFFIXES: .xml  .xml.in

all :: conf gencode exe data

# We Generated Code Ceneration XML files
.PHONY: conf
conf: $(XML_CONF_FILES)
	@echo "#################### XML Configuration Files Generation completed"

# We Generated XML data files
.PHONY: data
data: $(XML_DATA_FILES)
	@echo "#################### XML Data Files Generation completed"

# We Build executable files
.PHONY: exe
exe::
	@if [ ! -d $(PWD)/$(BUILD_TARGET) ]; then mkdir $(PWD)/$(BUILD_TARGET) ; fi
	@$(RM) $(PWD)/$(BUILD_TARGET)/Makefile
	@$(RM) $(PWD)/$(BUILD_TARGET)/Main.mk
	@ln -s $(PWD)/src/Makefile $(PWD)/$(BUILD_TARGET)/Makefile
	@ln -s $(PWD)/src/Main.mk $(PWD)/$(BUILD_TARGET)/Main.mk
	@$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory  all


# We Generated Code Sources files
.PHONY: gencode
gencode::
	@if [ ! -d $(PWD)/generated ]; then \
		mkdir $(PWD)/generated ; \
	fi
	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/Main.xml --dir=$(PWD)/generated --lang=cpp --validate
	@echo "#################### Code Generation completed"

# Rules to build each XML configuration file
%.xml: %.xml.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g > sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@orbacus_bin@%$(ORBACUS_BIN_PATH)%g >> sed_patterns
	@echo s%@orbacus_idl@%$(ORBACUS_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_IDL_PATH@%$(ORB_IDL_PATH)%g >> sed_patterns
	@echo s%@host_name@%`uname -n`%g >> sed_patterns
	@echo s%@host_name2@%`echo $(HOSTNAME2)`%g >> sed_patterns
	@echo s%@SystemMngt_port@%21`echo $PWD | wc -c | tail -c 2``id -u | tail -c 2``id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Monitoring_addr@%225.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@StateTransfert_addr@%226.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

clean::
	@if [ -d $(PWD)/$(BUILD_TARGET) ]; then \
		$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory  $@ ; \
	fi
	-$(RM) *.o core

cleanall: clean
	-$(RM) $(XML_DATA_FILES)
	-$(RM) -r $(PWD)/generated
	-$(RM) -r $(PWD)/$(BUILD_TARGET)

