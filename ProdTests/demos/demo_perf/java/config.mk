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

#################################################################################
# DO NOT EDIT !!!!!!
#################################################################################

# NOTA: This file requires prior inclusion of site.mk file
ifeq ($(strip $(CDMW_DEMO_HOME)),)
AbortMake:
	@echo "This file requires prior inclusion of site.mk file"
	@exit 1
endif

# -------------------------------------------------------------------------------
JAVAC:=javac
JAVA:=java
JAR:=jar

JDK_BIN:=$(shell which java | grep java | xargs dirname)
JDK_JAR:=$(shell echo $(JDK_BIN)/../jre/lib)

CDMW_CP=$(CDMW_HOME)/lib/java/libcdmwcommon.jar \
		$(CDMW_HOME)/lib/java/libcdmwossupport.jar \
		$(CDMW_HOME)/lib/java/libcdmworbsupport.jar \
		$(CDMW_HOME)/lib/java/libcdmwplatforminterface.jar \
		$(CDMW_HOME)/lib/java/libcdmwlifecycle.jar \
		$(CDMW_HOME)/lib/java/libcdmwcommonsvcsnaming.jar \
		$(CDMW_HOME)/lib/java/libcdmwrepositoryinterface.jar \
		$(CDMW_HOME)/lib/java/libcdmwrepositoryidl.jar \
		$(CDMW_HOME)/lib/java/libcdmwcdmwinit.jar \
		$(CDMW_HOME)/lib/java/libcdmweventinterface.jar \
		$(CDMW_HOME)/lib/java/libcdmweventsupport.jar \
		$(CDMW_HOME)/lib/java/libcdmwtracelibrary.jar \
		$(CDMW_HOME)/lib/java/libcdmwperformancelibrary.jar \
		$(CDMW_HOME)/lib/java/libcdmwsmginterfaceidl.jar

CDMW_LIB:=$(shell echo $(CDMW_CP) | sed 's/ /:/g')

XML_LIB=$(XERCES_JAR_PATH)/xercesImpl.jar:$(XERCES_JAR_PATH)/xmlParserAPIs.jar:$(XALAN_JAR_PATH)/xalan.jar:$(XALAN_JAR_PATH)/xml-apis.jar
                   
CLASSPATH=$(CDMW_DEMO_HOME)/prod:$(CDMW_LIB):$(ORB_LIB):$(XML_LIB)

RM=rm -f

