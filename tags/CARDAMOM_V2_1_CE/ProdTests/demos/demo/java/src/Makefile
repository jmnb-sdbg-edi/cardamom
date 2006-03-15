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

CLIENT_NAME=client
SERVER1_NAME=server1
SERVER2_NAME=server2

SRCS_IDL_SVR=ProcessAgent.idl AnonymousResourceFactory.idl ResourceFactory.idl Echo.idl
SRCS_IDL_CLT=
SRCS_IDL_TIE=CF.idl

# CDMW Generated Code
CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.java
CDMW_GEN_SERVERS_COMMON_SRC=ResourceServantProvider.java
CDMW_GEN_SERVER1_SRC=SessionResourceFactoryImpl.java \
		SessionResourceFactoryImplAllocator.java \
		Server1CdmwInterface.java \
		EntityResourceFactoryImpl.java \
		EntityResourceFactoryImplAllocator.java 
CDMW_GEN_SERVER2_SRC=Server2CdmwInterface.java \
		ServiceResourceFactoryImpl.java \
		ServiceResourceFactoryImplAllocator.java \
		UnMngEntityResourceFactoryImpl.java \
		UnMngEntityResourceFactoryImplAllocator.java

# User Supplied Code
SERVERS_COMMON_SRC=ApplicationImpl.java \
		DeviceImpl.java \
		ProcessAgentFactoryImpl.java \
		ProcessAgentFactoryImplAllocator.java \
		ProcessAgentImpl.java \
		UserResourceServantProvider.java \
		USPAllocator.java \
	 	EchoImpl.java \
		ServerProcessControl.java

USER_CLIENT_SRC=ClientThread.java \
	MyProcessControl.java \
	SessionStrategyTest.java \
	ServiceStrategyTest.java \
	UnManagedEntityStrategyTest.java \
	UnManagedEntityStrategyRestartTest.java \
	EntityStrategyTest.java \
	EntityStrategyRestartTest.java \
	TestData.java \
	Client.java
#	EventServiceTest.java \

USER_SERVER1_SRC=Server1.java

USER_SERVER2_SRC=UserAnonymousResourceServantProvider.java \
	UASPAllocator.java \
	Server2.java

CLIENT_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	$(USER_CLIENT_SRC)

SERVER1_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER1_SRC) \
	$(CDMW_GEN_SERVERS_COMMON_SRC) \
	$(USER_SERVER1_SRC)

SERVER2_SRCS=$(SERVERS_COMMON_SRC) \
	$(CDMW_GEN_SERVER2_SRC) \
	$(CDMW_GEN_SERVERS_COMMON_SRC) \
	$(USER_SERVER2_SRC)

CLIENT_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_CLIENT_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_CLIENT_SRC))

SERVER1_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(SERVERS_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER1_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVERS_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_SERVER1_SRC))

SERVER2_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(SERVERS_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER2_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVERS_COMMON_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_SERVER2_SRC))


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
all: $(SRCS_IDL) $(SERVER1_NAME) $(SERVER2_NAME) $(CLIENT_NAME)


.PHONY: $(SERVER1_NAME)
.PHONY: $(SERVER2_NAME)
.PHONY: $(CLIENT_NAME)


$(SERVER1_NAME): $(MANIFEST_MF) $(SERVER1_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SERVER1_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating cdmwdemo.jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf cdmwdemo.jar meta-inf `find . -name '*.class'`;


$(SERVER2_NAME): $(MANIFEST_MF) $(SERVER2_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SERVER2_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating cdmwdemo.jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf cdmwdemo.jar meta-inf `find . -name '*.class'`;


$(CLIENT_NAME): $(MANIFEST_MF) $(CLIENT_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(CLIENT_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating cdmwdemo.jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf cdmwdemo.jar meta-inf `find . -name '*.class'`;


$(MANIFEST_MF): 
	@echo "Creating Manifest file"
	@if [ ! -d $(MANIFEST_MF_DIR) ]; then mkdir $(MANIFEST_MF_DIR) ; fi
	@\rm -f $@ 
	@echo "Manifest-Version: 1.0" > $@

clean::
	@cd $(PROD_RELATIVE); \
	-$(RM) $(SRCS_IDL) `find . -name '*.class' -o -name '*.java'` $(MANIFEST_MF_DIR)

cleanall: clean
	-$(RM) cdmwdemo.jar

# -----------------------------------------------

