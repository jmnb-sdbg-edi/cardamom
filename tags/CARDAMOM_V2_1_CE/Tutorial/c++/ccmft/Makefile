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

PWD:=$(shell pwd)

# Imported variables
#   
#   CDMW_HOME
include ../site.mk

BUILD_TARGET:=$(shell $(CDMW_HOME)/bin/config.guess)



.SUFFIXES:
#.SUFFIXES: .xml  .xml.in
.SUFFIXES: .xml  .xml.in .txt .txt.in .cad .cad.in .cpf .cpf.in .csd .csd.in


include ../rules.mk

XML_CONF_FILES:=xml/CodeGen.xml
#CodeGenClientCompSrv.xml  \
#	xml/CodeGenServerCompSrv.xml \
#	xml/CodeGenGroupCreator.xml

XML_DATA_FILES:=data/CdmwFaultToleranceManager_ccmft_conf.xml \
	data/CdmwFTSystemMngtGroupCreator_conf.xml \
	data/CdmwPlatformMngtSystemStart.xml \
	data/cdmw_nar_valid_ccm_ft.xml \
	data/client.xml \
	data/server1.xml \
	data/server2.xml \
	data/server3.xml \
	data/GroupCreator.xml \
	data/CdmwPlatformMngtDaemon_conf.xml \
	data/EventChannelConf_ccm_ft.xml \
	data/ScenarioCcmFt.xml \
	run/ccm_integration_commands.txt \
        data/ValidCcmFt.cad \
	data/Client.cpf \
	data/ServerWithFacet.cpf \
	data/Client.csd  \
	data/ServerWithEvents.csd  \
	data/ServerWithFacet.csd


all :: conf data gencode exe

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
	@if [ ! -d $(PWD)/$(BUILD_TARGET) ]; then mkdir -p $(PWD)/$(BUILD_TARGET) ; fi
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
	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/CodeGen.xml --dir=$(PWD)/generated --lang=cpp --validate
#	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/CodeGenClientCompSrv.xml --dir=$(PWD)/generated --lang=cpp --validate
#	@echo "#################### Code Generation For client completed"
#	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/CodeGenServerCompSrv.xml --dir=$(PWD)/generated --lang=cpp --validate
#	@echo "#################### Code Generation for Server completed"
#	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/CodeGenGroupCreator.xml --dir=$(PWD)/generated --lang=cpp --validate
#	@echo "#################### Code Generation for Group completed"



clean::
	@if [ -d $(PWD)/$(BUILD_TARGET) ]; then \
		$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory  $@ ; \
	fi
	-$(RM) *.o core

cleanall: clean
	-$(RM) $(XML_CONF_FILES) $(XML_DATA_FILES)
	-$(RM) -r $(PWD)/generated
	-$(RM) -r $(PWD)/$(BUILD_TARGET)

