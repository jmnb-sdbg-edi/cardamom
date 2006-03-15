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



# Rules to build each XML configuration file
%.xml: %.xml.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@host_name@%`uname -n`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@CDMW_HOME@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@cdmwPlatformDaemonPort@%$(CDMW_PLATFORM_DAEMON_PORT)%g >> sed_patterns
	@echo s%@cdmwFaultTolerancePort@%$(CDMW_FT_MANAGER_PORT)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@config.FtpJAVA.ftp.dir@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@hostname1val@%$(HOSTNAME1)%g >> sed_patterns
	@echo s%@hostname2val@%$(HOSTNAME2)%g >> sed_patterns
	@echo s%@hostname3val@%$(HOSTNAME3)%g >> sed_patterns
	@echo s%@tao_root@%$(TAO_ROOT)%g >> sed_patterns
	@echo s%@SystemMngt_port@%21`echo $PWD | wc -c | tail -c 2``id -u | tail -c 2``id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Monitoring_addr@%225.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@StateTransfert_addr@%226.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
	@echo s%@Service_addr@%227.`echo $PWD | wc -c | tail -c 2`.`id -u | tail -c 2`.`id -u -n | wc -c | tail -c 2`%g >> sed_patterns
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



# Rules to build each TXT configuration file
%.txt: %.txt.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
	@echo s%@cdmwFaultTolerancePort@%$(CDMW_FT_MANAGER_PORT)%g >> sed_patterns
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
	@echo "s%<!--@IF_TAO@-->%<!-- ..TAO..%g" >> sed_patterns
	@echo "s%<!--@ENDIF_TAO@-->%..TAO.. -->%g" >> sed_patterns
	@echo "s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%<!--@ENDIF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%@-x-@%--%g" >> sed_patterns
endif
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns


# Rules to build each CAD configuration file
%.cad:%.cad.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
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
	@echo "s%<!--@IF_TAO@-->%<!-- ..TAO..%g" >> sed_patterns
	@echo "s%<!--@ENDIF_TAO@-->%..TAO.. -->%g" >> sed_patterns
	@echo "s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%<!--@ENDIF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%@-x-@%--%g" >> sed_patterns
endif
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns


# Rules to build each CPF configuration file
%.cpf:%.cpf.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@hostname1val@%$(HOSTNAME1)%g >> sed_patterns
	@echo s%@hostname2val@%$(HOSTNAME2)%g >> sed_patterns
	@echo s%@hostname3val@%$(HOSTNAME3)%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
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
	@echo "s%<!--@IF_TAO@-->%<!-- ..TAO..%g" >> sed_patterns
	@echo "s%<!--@ENDIF_TAO@-->%..TAO.. -->%g" >> sed_patterns
	@echo "s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%<!--@ENDIF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%@-x-@%--%g" >> sed_patterns
endif
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns



# Rules to build each CSD configuration file
%.csd:%.csd.in
	@echo "Generating $@ from $<"
	@echo s%@prefix@%$(PWD)%g >> sed_patterns
	@echo s%@hostname@%`hostname`%g >> sed_patterns
	@echo s%@cdmw_platform@%$(BUILD_TARGET)%g >> sed_patterns
	@echo s%@cdmw_home@%$(CDMW_HOME)%g >> sed_patterns
	@echo s%@FTP_DIR@%$(FTP_DIR)%g >> sed_patterns
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
	@echo "s%<!--@IF_TAO@-->%<!-- ..TAO..%g" >> sed_patterns
	@echo "s%<!--@ENDIF_TAO@-->%..TAO.. -->%g" >> sed_patterns
	@echo "s%<!--@IF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%<!--@ENDIF_ORBACUS@-->%<!-- ..ORBACUS.. -->%g" >> sed_patterns
	@echo "s%@-x-@%--%g" >> sed_patterns
endif
	@cat  $< | sed -f sed_patterns > $@
	@rm -f sed_patterns
