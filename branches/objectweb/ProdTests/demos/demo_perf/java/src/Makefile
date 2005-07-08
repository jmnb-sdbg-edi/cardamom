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
SERVER_NAME=server

SRCS_IDL_SVR=Benchmark.idl
SRCS_IDL_CLT=
SRCS_IDL_TIE=

# CDMW Generated Code
CDMW_GEN_CLIENT_SRC=ClientCdmwInterface.java
CDMW_GEN_SERVER_SRC=ServerCdmwInterface.java
		

# User Supplied Code
USER_SERVER_SRC=IImpl.java TesterImpl.java DataProvider.java \
    ValueImpl.java ValueFactoryImpl.java \
    ServerProcessControl.java Server.java

USER_CLIENT_SRC=IImpl.java BenchmarkThread.java DataProvider.java \
    ValueImpl.java ValueFactoryImpl.java \
	ClientProcessControl.java Client.java

CLIENT_SRCS=$(CDMW_GEN_CLIENT_SRC) \
	$(USER_CLIENT_SRC)

SERVER_SRCS=$(CDMW_GEN_SERVER_SRC) \
	$(USER_SERVER_SRC)


CLIENT_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_CLIENT_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_CLIENT_SRC))

SERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(CDMW_GEN_SERVER_SRC)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_SERVER_SRC))


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
all: $(SRCS_IDL) $(SERVER_NAME) $(CLIENT_NAME)


.PHONY: $(SERVER_NAME)
.PHONY: $(CLIENT_NAME)


$(SERVER_NAME): $(MANIFEST_MF) $(SERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating cdmwdemoperf.jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf cdmwdemoperf.jar meta-inf `find . -name '*.class'`;


$(CLIENT_NAME): $(MANIFEST_MF) $(CLIENT_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(CLIENT_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating cdmwdemoperf.jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf cdmwdemoperf.jar meta-inf `find . -name '*.class'`;


$(MANIFEST_MF): 
	@echo "Creating Manifest file"
	@if [ ! -d $(MANIFEST_MF_DIR) ]; then mkdir $(MANIFEST_MF_DIR) ; fi
	@\rm -f $@ 
	@echo "Manifest-Version: 1.0" > $@

clean::
	@cd $(PROD_RELATIVE); \
	-$(RM) $(SRCS_IDL) `find . -name '*.class' -o -name '*.java'` $(MANIFEST_MF_DIR)

cleanall: clean
	-$(RM) cdmwdemoperf.jar

# -----------------------------------------------

