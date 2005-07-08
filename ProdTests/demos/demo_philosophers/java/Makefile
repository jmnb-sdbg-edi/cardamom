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
include site.mk

BUILD_TARGET:=prod/$(ORB)

XML_CONF_FILES:=xml/ComponentServers.xml

XML_DATA_FILES:=run/ccm_integration_commands.txt run/start_interface_repository.sh run/define_system.sh \
	run/make_assembly_package.sh run/start_demo.sh run/start_daemon.sh run/start_supervision.sh \
	run/start_system.sh run/stop_system.sh data/Dinner.cad data/EventChannelConf1.xml \
	data/FrenchSchoolComponentServer.xml data/GreekSchoolComponentServer.xml data/ObserverComponentServer.xml \
	data/integ_deployment.xml data/cdmw_nar_integ_ccm.xml

.SUFFIXES:
.SUFFIXES: .xml .xml.in .sh .sh.in .txt .txt.in .cad .cad.in

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
	@cp $(PWD)/src/Makefile $(PWD)/$(BUILD_TARGET)/Makefile
	@$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory all
	@echo "#################### Java source files Compilation completed"


# We Generated Code Sources files
.PHONY: gencode
gencode::
	@if [ ! -d $(PWD)/generated ]; then \
		mkdir $(PWD)/generated ; \
	fi
	$(CDMW_HOME)/bin/cdmw_code_generator.sh --xml=$(PWD)/xml/ComponentServers.xml --dir=$(PWD)/generated --lang=java --validate
	@echo "#################### Code Generation completed"

# Rules to build each XML configuration file
%.xml: %.xml.in
	@echo "Generating $@ from $<"
	@echo s%@runtime_root@%$(CDMW_RUNTIME_ROOT)%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@JACORB_IDL_PATH@%$(JACORB_IDL_PATH)%g >> sed_patterns
	@echo s%@JACORB_JAR_PATH@%$(JACORB_JAR_PATH)%g >> sed_patterns
	@echo s%@JACORB_BIN_PATH@%$(JACORB_BIN_PATH)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_BIN_PATH@%$(ORBACUS_JAVA_BIN_PATH)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_IDL_PATH@%$(ORBACUS_JAVA_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_JAR_PATH@%$(ORB_JAR_PATH)%g >> sed_patterns
	@echo s%@XERCES_JAR_PATH@%$(XERCES_JAR_PATH)%g >> sed_patterns
	@echo s%@XALAN_JAR_PATH@%$(XALAN_JAR_PATH)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

# Rules to build each sh configuration file
%.sh: %.sh.in
	@echo "Generating $@ from $<"
	@echo s%@runtime_root@%$(CDMW_RUNTIME_ROOT)%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_BIN_PATH@%$(ORBACUS_JAVA_BIN_PATH)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_IDL_PATH@%$(ORBACUS_JAVA_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_JAR_PATH@%$(ORB_JAR_PATH)%g >> sed_patterns
	@echo s%@XERCES_JAR_PATH@%$(XERCES_JAR_PATH)%g >> sed_patterns
	@echo s%@XALAN_JAR_PATH@%$(XALAN_JAR_PATH)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns
	@chmod a+x $@

# Rules to build each TXT configuration file
%.txt: %.txt.in
	@echo "Generating $@ from $<"
	@echo s%@runtime_root@%$(CDMW_RUNTIME_ROOT)%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_BIN_PATH@%$(ORBACUS_JAVA_BIN_PATH)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_IDL_PATH@%$(ORBACUS_JAVA_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_JAR_PATH@%$(ORB_JAR_PATH)%g >> sed_patterns
	@echo s%@XERCES_JAR_PATH@%$(XERCES_JAR_PATH)%g >> sed_patterns
	@echo s%@XALAN_JAR_PATH@%$(XALAN_JAR_PATH)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

%.cad:%.cad.in
	@echo "Generating $@ from $<"
	@echo s%@runtime_root@%$(CDMW_RUNTIME_ROOT)%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_BIN_PATH@%$(ORBACUS_JAVA_BIN_PATH)%g >> sed_patterns
	@echo s%@ORBACUS_JAVA_IDL_PATH@%$(ORBACUS_JAVA_IDL_PATH)%g >> sed_patterns
	@echo s%@ORB_JAR_PATH@%$(ORB_JAR_PATH)%g >> sed_patterns
	@echo s%@XERCES_JAR_PATH@%$(XERCES_JAR_PATH)%g >> sed_patterns
	@echo s%@XALAN_JAR_PATH@%$(XALAN_JAR_PATH)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns

clean::
	@if [ -d $(PWD)/$(BUILD_TARGET) ]; then \
		$(MAKE) -C $(PWD)/$(BUILD_TARGET) --no-print-directory  $@ ; \
	fi
	$(RM) *.o core

cleanall: clean
	$(RM) $(XML_CONF_FILES) $(XML_DATA_FILES)
	$(RM) -r $(PWD)/generated
	$(RM) -r $(PWD)/$(BUILD_TARGET)

