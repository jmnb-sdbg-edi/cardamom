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
CODE_GEN_DIR_RELATIV_PROD=../..

include $(CODE_GEN_DIR_RELATIV_PROD)/site.mk
include $(CODE_GEN_DIR_RELATIV_PROD)/$(ORB).mk
include $(CODE_GEN_DIR_RELATIV_PROD)/config.mk
PROD_RELATIVE=$(CODE_GEN_DIR_RELATIV_PROD)/prod/$(ORB)

# Manifest used to produced the JAR
MANIFEST_MF_DIR:=$(PROD_RELATIVE)/meta-inf
MANIFEST_MF:=$(MANIFEST_MF_DIR)/Manifest.mf

# Append to IDLFLAGS
override JIDLFLAGS+= $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/idl $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/generated
CP:=$(CLASSPATH):.:$(CODE_GEN_DIR_RELATIV_PROD)/generated

vpath %.idl $(CODE_GEN_DIR_RELATIV_PROD)/idl:$(CODE_GEN_DIR_RELATIV_PROD)/generated
vpath %.java $(CODE_GEN_DIR_RELATIV_PROD)/src:$(CODE_GEN_DIR_RELATIV_PROD)/generated

SERVER1_NAME=componentServer1
SERVER2_NAME=componentServer2
LIB_CLIENT_NAME=libClientBasicDemo
LIB_SERVER_NAME=libServerBasicDemo

SRCS_IDL_SVR=UserBasicDemo.idl
SRCS_IDL_CLT=
SRCS_IDL_TIE= $(shell find $(CODE_GEN_DIR_RELATIV_PROD)/generated -name '*.idl' -exec basename {} \;)

# CDMW Generated Code
CDMW_GEN_CLIENT_COMMON_SRC=
CDMW_GEN_CLIENT_SRC=
CDMW_GEN_LIB_CLIENT_SRC=

CDMW_GEN_SERVER_COMMON_SRC=CCM_Client_SessionContextImpl.java CdmwInterface1.java \
	SessionClientHome_impl.java SessionClientImpl.java
CDMW_GEN_SERVER1_SRC=CCM_Server_SessionContextImpl.java \
	SessionServerHome_impl.java SessionServerImpl.java DisplayImpl.java
CDMW_GEN_SERVER2_SRC=CdmwInterface2.java
CDMW_GEN_LIB_SERVER_SRC=

# User Supplied Code
SERVERS_COMMON_SRC=

LIB_CLIENT_SRCS=CCMClientHomeImpl.java CCMClientImpl.java EventImpl.java EventFactoryImpl.java

LIB_SERVER_SRCS=CCMDisplayImpl.java CCMServerHomeImpl.java CCMServerImpl.java

SERVER1_SRCS=MainComponentServer1.java

SERVER2_SRCS=MainComponentServer2.java

LIB_CLIENT_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_CLIENT_SRCS))

LIB_SERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_SERVER_SRCS))

SERVER1_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER1_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(SERVER1_SRCS))

SERVER2_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER2_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(SERVER2_SRCS))


.SUFFIXES:
.SUFFIXES: .idl .java .jar .class .client .server .tie

# Default rule to generate the server files from the idl
%.server : %.idl
	$(JIDL_SVR) $(JIDLFLAGS) $(JIDL_FLAGS_SVR) $< && touch $@

# Default rule to generate the client files from the idl
%.client : %.idl
	$(JIDL_CLT) $(JIDLFLAGS) $(JIDL_FLAGS_CLT) $< && touch $@

# Default rule to generate the server files from the idl with tie classes
%.tie : %.idl
	$(JIDL_SVR) $(JIDLFLAGS) $(JIDL_FLAGS_TIE) $< && touch $@

SRCS_IDL_SVR_CLIENT=$(SRCS_IDL_SVR:.idl=.client)
SRCS_IDL_SVR_SERVER=$(SRCS_IDL_SVR:.idl=.server)
SRCS_IDL_TIE_CLIENT=$(SRCS_IDL_TIE:.idl=.client)
SRCS_IDL_TIE_SERVER=$(SRCS_IDL_TIE:.idl=.tie)
SRCS_IDL_CLT_CLIENT=$(SRCS_IDL_CLT:.idl=.client)

# All the files to be generated
SRCS_IDL=$(SRCS_IDL_SVR_CLIENT) $(SRCS_IDL_SVR_SERVER) $(SRCS_IDL_CLT_CLIENT) $(SRCS_IDL_TIE_SERVER) $(SRCS_IDL_TIE_CLIENT)


.PHONY: all
all: $(SRCS_IDL) $(LIB_SERVER_NAME)  $(LIB_CLIENT_NAME) $(SERVER2_NAME) $(SERVER1_NAME)
	@echo "Create BasicDemo package"
	$(CDMW_DEMO_HOME)/run/make_assembly_package.sh


.PHONY: $(SERVER1_NAME)
.PHONY: $(SERVER2_NAME)
.PHONY: $(LIB_CLIENT_NAME)
.PHONY: $(LIB_SERVER_NAME)


$(SERVER1_NAME): $(MANIFEST_MF) $(SERVER1_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SERVER1_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(SERVER1_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(SERVER1_NAME).jar meta-inf `find . -name '*.class'`;


$(SERVER2_NAME): $(MANIFEST_MF) $(SERVER2_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SERVER2_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(SERVER2_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(SERVER2_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_CLIENT_NAME): $(MANIFEST_MF) $(LIB_CLIENT_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_CLIENT_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_CLIENT_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_CLIENT_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_SERVER_NAME): $(MANIFEST_MF) $(LIB_SERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_SERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_SERVER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_SERVER_NAME).jar meta-inf `find . -name '*.class'`;


$(MANIFEST_MF): 
	@echo "Creating Manifest file"
	@if [ ! -d $(MANIFEST_MF_DIR) ]; then mkdir $(MANIFEST_MF_DIR) ; fi
	@\rm -f $@ 
	@echo "Manifest-Version: 1.0" > $@

clean::
	@cd $(PROD_RELATIVE); \
	-$(RM) $(SRCS_IDL) `find . -name '*.class' -o -name '*.java'` $(MANIFEST_MF_DIR)

cleanall: clean
	-$(RM) $(SERVER1_NAME).jar
	-$(RM) $(SERVER2_NAME).jar
	-$(RM) $(LIB_CLIENT_NAME).jar
	-$(RM) $(LIB_SERVER_NAME).jar

# -----------------------------------------------

