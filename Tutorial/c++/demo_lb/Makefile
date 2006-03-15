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




.SUFFIXES: .sh .sh.in .xml  .xml.in .txt .txt.in 


include ../rules.mk

XML_CONF_FILES:=xml/CodeGen.xml  

XML_DATA_FILES:=data/cdmw_nar_demo_lb.xml \
	data/CdmwFTSystemMngtGroupCreator_conf.xml \
        data/HelloLoadBalancing.xml \
        data/LBClient.xml \
	run/command_proc1_stop.sh \
        run/command_proc2_stop.sh \
        run/command_proc1_initialise.sh \
	run/command_proc1_run.sh \
        run/command_app_stop.sh \
        data/ScenarioDemoLB.xml \
        run/launchLoadBalancingClient.sh 

all :: conf gencode exe data 

# We Generated Code Ceneration XML files
.PHONY: conf
conf: $(XML_CONF_FILES)
	@echo "#################### XML Configuration Files Generation completed" 

# We Generated XML data files
.PHONY: data
data: $(XML_DATA_FILES) script
	@echo "#################### XML Data Files Generation completed" 

.PHONY: script
script::
	@chmod +x `find . -name "*.sh"` 

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

# Rules to build each sh file
%.sh: %.sh.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@config.Ftp.homedir@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@config.FtpJAVA.ftp.dir@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@hostname1val@%$(HOSTNAME1)%g >> sed_patterns
	@echo s%@hostname2val@%$(HOSTNAME2)%g >> sed_patterns
	@echo s%@hostname3val@%$(HOSTNAME3)%g >> sed_patterns
ifeq (${ORB_CPP_NAME},tao)
	@echo s%@ORB_IDL_PATH@%$(ORB_IDL_PATH)%g >> sed_patterns
	@echo "s%<!--@IF_TAO@-->%<!-- ..TAO.. -->%g" >> sed_patterns
	@echo "s%<!--@ENDIF_TAO@-->%<!-- ..TAO.. -->%g" >> sed_patterns
	@echo "s%@-xx-@%--%g" >> sed_patterns
	@echo "s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS..%g" >> sed_patterns
	@echo "s%<!--@ENDIF_ORBACUS@-->%..ORBACUS.. -->%g" >> sed_patterns
endif
ifeq (${ORB_CPP_NAME},orbacus)
	@echo s%@ORB_BIN_PATH@%$(ORBACUS_BIN_PATH)%g >> sed_patterns
	@echo s%@ORB_IDL_PATH@%$(ORBACUS_IDL_PATH)%g >> sed_patterns
	@echo s%<!--@IF_TAO@-->%<!-- ..TAO..%g >> sed_patterns
	@echo s%<!--@ENDIF_TAO@-->%..TAO.. -->%g >> sed_patterns
	@echo s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g >> sed_patterns
	@echo s%<!--@ENDIF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g >> sed_patterns
	@echo s%@-x-@%--%g >> sed_patterns
endif
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns


# Rules to build each XML configuration file
%.xml: %.xml.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g > sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@config.Ftp.homedir@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@ORB_IDL_PATH@%$(ORB_IDL_PATH)%g>> sed_patterns
	@echo s%@PWD@%$(PWD)%g >> sed_patterns
	@echo s%@CDMW_HOME@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@GENERATION_DIR@%$(GENERATION_DIR)%g >> sed_patterns
	@echo s%@DELIVERY_DIR@%$(DELIVERY_DIR)%g >> sed_patterns
	@echo s%@TAO_ROOT@%$(TAO_ROOT)%g >> sed_patterns
	@echo s%@hostname1val@%$(HOSTNAME1)%g >> sed_patterns
	@echo s%@hostname2val@%$(HOSTNAME2)%g >> sed_patterns
	@echo s%@hostname3val@%$(HOSTNAME3)%g >> sed_patterns
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

