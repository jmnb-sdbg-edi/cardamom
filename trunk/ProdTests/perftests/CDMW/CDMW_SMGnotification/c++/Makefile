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

PWD:=$(shell pwd)


# Imported variables
#   
#   CDMW_HOME
include site.mk

# Supervision port
CDMW_SVS_PORT:=21883

BUILD_TARGET:=$(shell $(CDMW_HOME)/bin/config.guess)

XML_CONF_FILES:=xml/Main.xml

XML_DATA_FILES:=data/Client.xml data/Server.xml \
	data/Scenario1H.xml data/Scenario2H.xml \
	data/Cdmw_nar.xml \
	data/CdmwPlatformMngtDaemon_conf.xml \
	data/CdmwFTSystemMngtGroupCreator_conf.xml \
	data/CdmwPlatformMngtSystemStart.xml \
	data/CdmwFaultToleranceManager_conf.xml
	
SHELL_SCRIPT_FILES:=run/start_single_test1H.sh \
		    run/start_single_test2H.sh	

.SUFFIXES:
.SUFFIXES: .xml  .xml.in 
.SUFFIXES: .sh .sh.in

all:: conf gencode exe data shell

# We Generated Code Ceneration XML files
.PHONY: conf
conf:: $(XML_CONF_FILES)
	@echo "#################### XML Configuration Files Generation completed"

# We Generated XML data files
.PHONY: data
data:: $(XML_DATA_FILES)
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

# We Generated Shell script .sh files
.PHONY: shell
shell:: $(SHELL_SCRIPT_FILES)
	@chmod +x $(PWD)/$(SHELL_SCRIPT_FILES)
	@echo "#################### Shell script Files Generation completed"

# Rules to build each XML configuration file
%.xml: %.xml.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g > sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@orbacus_bin@%$(ORBACUS_BIN_PATH)%g >> sed_patterns
	@echo s%@orbacus_idl@%$(ORBACUS_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_IDL_PATH@%$(ORB_IDL_PATH)%g>> sed_patterns
	@echo s%@hostname@%`hostname`%g>> sed_patterns
	@echo s%@hostname2@%`cat ../../Testparams.sh | grep -w REMOTEHOST |head -1|cut -d= -f2`%g>> sed_patterns
	@echo s%@SystemMngt_port@%21`echo $PWD | wc -c | tail -c 2``id -u | tail -c 2``id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Monitoring_addr@%225.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Service_addr@%227.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@StateTransfert_addr@%226.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_svs_port@%$(CDMW_SVS_PORT)%g >> sed_patterns	

	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

%.sh: %.sh.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g > sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@orbacus_bin@%$(ORBACUS_BIN_PATH)%g >> sed_patterns
	@echo s%@orbacus_idl@%$(ORBACUS_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_IDL_PATH@%$(ORB_IDL_PATH)%g>> sed_patterns
	@echo s%@hostname@%`hostname`%g>> sed_patterns
	@echo s%@hostname2@%`cat ../../Testparams.sh | grep -w REMOTEHOST |head -1|cut -d= -f2`%g>> sed_patterns
	@echo s%@SystemMngt_port@%21`echo $PWD | wc -c | tail -c 2``id -u | tail -c 2``id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Monitoring_addr@%225.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Service_addr@%227.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@StateTransfert_addr@%226.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_svs_port@%$(CDMW_SVS_PORT)%g >> sed_patterns	

	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

clean::
	@if [ -d $(PWD)/$(BUILD_TARGET) ]; then \
		$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory  $@ ; \
	fi
	-$(RM) *.o core

cleanall: clean
	-$(RM) $(XML_DATA_FILES)
	-$(RM) $(XML_CONF_FILES)
	-$(RM) $(SHELL_SCRIPT_FILES)
	-$(RM) -r $(PWD)/generated
	-$(RM) -r $(PWD)/$(BUILD_TARGET)

