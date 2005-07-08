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


GREEK_SERVER_NAME=greek_school_component_server
FRENCH_SERVER_NAME=french_school_component_server
OBSERVER_SERVER_NAME=observer_component_server

LIB_PHILOSOPHER_NAME=libPhilosopher
LIB_FORK_MANAGER_NAME=libForkManager
LIB_OBSERVER_NAME=libObserver
LIB_REGISTRATOR_NAME=libRegistrator

SRCS_IDL_SVR=UserPhilo.idl
SRCS_IDL_CLT=
SRCS_IDL_TIE= $(shell find $(CODE_GEN_DIR_RELATIV_PROD)/generated -name '*.idl' -exec basename {} \;)


# user provided src

LIB_PHILOSOPHER_SRCS=\
    CCMPhilosopherHomeImpl.java CCMPhilosopherImpl.java \
    StatusInfoFactoryImpl.java StatusInfoImpl.java

LIB_FORK_MANAGER_SRCS=\
    CCMForkImpl.java CCMForkHomeImpl.java CCMForkManagerImpl.java

LIB_OBSERVER_SRCS=\
    CCMObserverHomeImpl.java CCMObserverImpl.java \
    StatusInfoFactoryImpl.java StatusInfoImpl.java
    
LIB_REGISTRATOR_SRCS=\
    CCMRegistratorHomeImpl.java CCMRegistratorImpl.java

# generated src

GENERATED_PHILOSOPHER_SRCS=\
    CCM_Philosopher_SessionContextImpl.java \
    SessionPhilosopherImpl.java SessionPhilosopherHome_impl.java
    
GENERATED_FORK_MANAGER_SRCS=\
    CCM_ForkManager_SessionContextImpl.java \
    SessionForkManagerImpl.java SessionForkHome_impl.java  \
    ForkImpl.java

GENERATED_OBSERVER_SRCS=\
    CCM_Observer_SessionContextImpl.java  \
    SessionObserverImpl.java  SessionObserverHome_impl.java \
    StatusInfoConsumerImpl.java

GENERATED_REGISTRATOR_SRCS=\
    CCM_Registrator_SessionContextImpl.java \
    SessionRegistratorImpl.java SessionRegistratorHome_impl.java

GENERATED_GREEK_SERVER_SRCS=\
    GreekSchoolCdmwInterface.java

GENERATED_FRENCH_SERVER_SRCS=\
    FrenchSchoolCdmwInterface.java

GENERATED_OBSERVER_SERVER_SRCS=\
    ObserverCdmwInterface.java

# servers src

GREEK_SERVER_SRCS=GreekSchoolComponentServer.java

FRENCH_SERVER_SRCS=FrenchSchoolComponentServer.java

OBSERVER_SERVER_SRCS=ObserverComponentServer.java


LIB_PHILOSOPHER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_PHILOSOPHER_SRCS))

LIB_FORK_MANAGER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_FORK_MANAGER_SRCS))

LIB_OBSERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_OBSERVER_SRCS))

LIB_REGISTRATOR_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(LIB_REGISTRATOR_SRCS))


GREEK_SERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(GREEK_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_GREEK_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_PHILOSOPHER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_FORK_MANAGER_SRCS))

FRENCH_SERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(FRENCH_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_FRENCH_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_PHILOSOPHER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_FORK_MANAGER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_REGISTRATOR_SRCS))

OBSERVER_SERVER_SRCS_RELATIV=$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/src/, $(OBSERVER_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_OBSERVER_SERVER_SRCS)) \
	$(addprefix $(CODE_GEN_DIR_RELATIV_PROD)/generated/, $(GENERATED_OBSERVER_SRCS))


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
all: $(SRCS_IDL) $(LIB_PHILOSOPHER_NAME) $(LIB_FORK_MANAGER_NAME) $(LIB_OBSERVER_NAME) $(LIB_REGISTRATOR_NAME) $(FRENCH_SERVER_NAME) $(GREEK_SERVER_NAME) $(OBSERVER_SERVER_NAME)
	@echo "Create Dinner package"
	$(CDMW_DEMO_HOME)/run/make_assembly_package.sh


.PHONY: $(FRENCH_SERVER_NAME)
.PHONY: $(GREEK_SERVER_NAME)
.PHONY: $(OBSERVER_SERVER_NAME)
.PHONY: $(LIB_PHILOSOPHER_NAME)
.PHONY: $(LIB_FORK_MANAGER_NAME)
.PHONY: $(LIB_OBSERVER_NAME)
.PHONY: $(LIB_REGISTRATOR_NAME)


$(FRENCH_SERVER_NAME): $(MANIFEST_MF) $(FRENCH_SERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(FRENCH_SERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(FRENCH_SERVER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(FRENCH_SERVER_NAME).jar meta-inf `find . -name '*.class'`;


$(GREEK_SERVER_NAME): $(MANIFEST_MF) $(GREEK_SERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(GREEK_SERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(GREEK_SERVER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(GREEK_SERVER_NAME).jar meta-inf `find . -name '*.class'`;


$(OBSERVER_SERVER_NAME): $(MANIFEST_MF) $(OBSERVER_SERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(OBSERVER_SERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(OBSERVER_SERVER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(OBSERVER_SERVER_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_PHILOSOPHER_NAME): $(MANIFEST_MF) $(LIB_PHILOSOPHER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_PHILOSOPHER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_PHILOSOPHER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_PHILOSOPHER_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_FORK_MANAGER_NAME): $(MANIFEST_MF) $(LIB_FORK_MANAGER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_FORK_MANAGER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_FORK_MANAGER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_FORK_MANAGER_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_OBSERVER_NAME): $(MANIFEST_MF) $(LIB_OBSERVER_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_OBSERVER_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_OBSERVER_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_OBSERVER_NAME).jar meta-inf `find . -name '*.class'`;


$(LIB_REGISTRATOR_NAME): $(MANIFEST_MF) $(LIB_REGISTRATOR_SRCS_RELATIV) $(SRCS_IDL)
	CLASSPATH=$(CP) \
	export CLASSPATH; \
	$(JAVAC) -bootclasspath $$CLASSPATH:$(JDK_JAR)/rt.jar -d $(PROD_RELATIVE) $(LIB_REGISTRATOR_SRCS_RELATIV) `find $(PROD_RELATIVE) -name '*.java'`;
	@echo Creating $(LIB_REGISTRATOR_NAME).jar archive
	@cd $(PROD_RELATIVE); \
	$(JAR) cvMf $(LIB_REGISTRATOR_NAME).jar meta-inf `find . -name '*.class'`;


$(MANIFEST_MF): 
	@echo "Creating Manifest file"
	@if [ ! -d $(MANIFEST_MF_DIR) ]; then mkdir $(MANIFEST_MF_DIR) ; fi
	@\rm -f $@ 
	@echo "Manifest-Version: 1.0" > $@

clean::
	@cd $(PROD_RELATIVE); \
	-$(RM) $(SRCS_IDL) `find . -name '*.class' -o -name '*.java'` $(MANIFEST_MF_DIR)

cleanall: clean
	-$(RM) $(FRENCH_SERVER_NAME).jar
	-$(RM) $(GREEK_SERVER_NAME).jar
	-$(RM) $(OBSERVER_SERVER_NAME).jar
	-$(RM) $(LIB_PHILOSOPHER_NAME).jar
	-$(RM) $(LIB_FORK_MANAGER_NAME).jar
	-$(RM) $(LIB_OBSERVER_NAME).jar
	-$(RM) $(LIB_REGISTRATOR_NAME).jar


# -----------------------------------------------

