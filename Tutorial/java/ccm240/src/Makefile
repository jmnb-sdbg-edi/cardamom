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
CODE_GEN_DIR_RELATIV_PROD=..

include $(CODE_GEN_DIR_RELATIV_PROD)/../site.mk
include $(CODE_GEN_DIR_RELATIV_PROD)/../$(ORB).mk
include $(CODE_GEN_DIR_RELATIV_PROD)/../config.mk
BUILD_TARGET=$(shell $(CDMW_HOME)/bin/config.guess)
PROD_RELATIVE=$(CODE_GEN_DIR_RELATIV_PROD)/$(BUILD_TARGET)
GEN_RELATIVE=$(CODE_GEN_DIR_RELATIV_PROD)/generated

include $(CODE_GEN_DIR_RELATIV_PROD)/src/CSC.mk
CDMW_CP:=$(foreach CSC,$(CSC_DEPEND_LIB),$(CDMW_HOME)/lib/java/libcdmw$(CSC).jar)
CDMW_LIB:=$(shell echo $(CDMW_CP) | sed 's/ /:/g')
CLASSPATH=$(PWD)/prod:$(CDMW_LIB):$(ORB_LIB):$(XML_LIB)

# Append to IDLFLAGS
override JIDLFLAGS+= --tie $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/idl $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/generated

vpath %.idl $(CODE_GEN_DIR_RELATIV_PROD)/idl:$(CODE_GEN_DIR_RELATIV_PROD)/generated
vpath %.java $(CODE_GEN_DIR_RELATIV_PROD)/src:$(CODE_GEN_DIR_RELATIV_PROD)/generated

# Jar to be generated
SERVER_JAR=componentserver.jar
SERVER_JAR_FILES=`cd $(PROD_RELATIVE)` \
                 `find com/ -name '*.class'` \
                 `find cdmw/ -name '*.class'` \
                 `find Cdmw/ -name '*.class'` \
                 `cd -` \
                  Hello/MainComponentServer1.class \
                  Hello/MainComponentServer2.class

LIB_CLIENT_JAR=libclient.jar
LIB_CLIENT_JAR_FILES=Hello/ClientExecutorImpl.class \
                     Hello/ClientActivationThread.class \
                     Hello/ClientHomeExecutorImpl.class \
                     Hello/ClientHomeFactory.class \
                     Hello/HelloInfoImpl.class \
                     Hello/HelloInfoFactory.class

LIB_SERVER_JAR=libserver.jar
LIB_SERVER_JAR_FILES=Hello/ServerExecutorImpl.class \
                     Hello/ServerHomeExecutorImpl.class \
                     Hello/ServerHomeFactory.class \
                     Hello/DisplayFacetExecutorImpl.class \
                     Hello/HelloInfoImpl.class



# User Supplied idl
SRCS_IDL=Cdmw_Hello_cif.idl Hello_cif.idl UserHello.idl 

# User Supplied Code
USER_SRC=ClientExecutorImpl.java ClientActivationThread.java ClientHomeExecutorImpl.java \
	     ClientHomeFactory.java MainComponentServer1.java \
         DisplayFacetExecutorImpl.java HelloInfoImpl.java HelloInfoFactory.java \
         ServerExecutorImpl.java ServerHomeExecutorImpl.java \
	     ServerHomeFactory.java MainComponentServer2.java


SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(USER_SRC))



.SUFFIXES:
.SUFFIXES: .idl .java .jar .class .client .server .tie

# Default rule to generate the server files from the idl
%.server : %.idl
	@echo JIDLFLAGS = $(JIDLFLAGS)
	$(JIDL_SVR) $(JIDLFLAGS) $< && touch $@


SRCS_IDL_SERVER=$(SRCS_IDL:.idl=.server)


# All the files to be generated
SRCS_GENERATED=$(SRCS_IDL_SERVER)


.PHONY: all
all: $(SRCS_GENERATED) compile $(SERVER_JAR) $(LIB_CLIENT_JAR) $(LIB_SERVER_JAR)
	@echo 
	@echo "#################### Create Hello package"
	../run/make_assembly_package.sh


.PHONY: $(JAR_NAME)


compile: $(SRCS_RELATIV) $(SRCS_GENERATED)
	@echo "CLASSPATH=$(CLASSPATH)"
	$(JAVAC) -bootclasspath $(CLASSPATH):$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'` `find $(GEN_RELATIVE) -name '*.java'`;


$(SERVER_JAR): $(SRCS_RELATIV) $(SRCS_GENERATED) compile
	@echo 
	@echo "#################### Create $(SERVER_JAR)"
	@cd $(PROD_RELATIVE); \
	$(JAR) cvf $(SERVER_JAR) $(SERVER_JAR_FILES); \
	cd -;

$(LIB_CLIENT_JAR): $(SRCS_RELATIV) $(SRCS_GENERATED) compile
	@echo 
	@echo "#################### Create $(LIB_CLIENT_JAR)"
	@cd $(PROD_RELATIVE); \
	$(JAR) cvf $(LIB_CLIENT_JAR) $(LIB_CLIENT_JAR_FILES); \
	cd -;
    
$(LIB_SERVER_JAR): $(SRCS_RELATIV) $(SRCS_GENERATED) compile
	@echo 
	@echo "#################### Create $(LIB_SERVER_JAR)"
	@cd $(PROD_RELATIVE); \
	$(JAR) cvf $(LIB_SERVER_JAR) $(LIB_SERVER_JAR_FILES); \
	cd -;
    


clean::
	@cd $(PROD_RELATIVE); \
	$(RM) $(SRCS_GENERATED) `find . -name '*.class' -o -name '*.java'`

cleanall: clean
	@$(RM) $(JAR_NAME)

# -----------------------------------------------


