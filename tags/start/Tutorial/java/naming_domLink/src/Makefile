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
override JIDLFLAGS+= $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/idl $(JIDL_INCLUDE_DIR_OPTION)$(CODE_GEN_DIR_RELATIV_PROD)/generated

vpath %.idl $(CODE_GEN_DIR_RELATIV_PROD)/idl:$(CODE_GEN_DIR_RELATIV_PROD)/generated
vpath %.java $(CODE_GEN_DIR_RELATIV_PROD)/src:$(CODE_GEN_DIR_RELATIV_PROD)/generated

# Jar to be generated
JAR_NAME=cdmwtutorial-naming_domlink.jar

# User Supplied idl
SRCS_IDL=Hello.idl

# User Supplied Code
USER_SRC=HelloInterfaceImpl.java HelloServer.java HelloClient.java 


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
all: $(SRCS_GENERATED) $(JAR_NAME)


.PHONY: $(JAR_NAME)


$(JAR_NAME): $(SRCS_RELATIV) $(SRCS_GENERATED)
	@echo "CLASSPATH=$(CLASSPATH)"
	$(JAVAC) -bootclasspath $(CLASSPATH):$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'` `find $(GEN_RELATIVE) -name '*.java'`;
	@echo Creating $(JAR_NAME) archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvf $(JAR_NAME) `find . -name '*.class'`;



clean::
	@cd $(PROD_RELATIVE); \
	$(RM) $(SRCS_GENERATED) `find . -name '*.class' -o -name '*.java'`

cleanall: clean
	@$(RM) $(JAR_NAME)

# -----------------------------------------------


