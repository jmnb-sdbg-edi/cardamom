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
ifeq ($(strip $(CDMW_TUTORIAL_HOME)),)
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

#CDMW_CP=$(CDMW_HOME)/bin/cdmwcommon.jar $(CDMW_HOME)/bin/cdmwossupport.jar \
#	$(CDMW_HOME)/bin/cdmworbsupport.jar $(CDMW_HOME)/bin/cdmwplatforminterface.jar \
#	$(CDMW_HOME)/bin/cdmwidl.jar $(CDMW_HOME)/bin/cdmwlifecycle.jar \
#	$(CDMW_HOME)/bin/cdmwnaminginterface.jar $(CDMW_HOME)/bin/cdmwrepositoryinterface.jar \
#	$(CDMW_HOME)/bin/cdmwcdmwinit.jar $(CDMW_HOME)/bin/cdmweventinterface.jar \
#	$(CDMW_HOME)/bin/cdmwtracelibrary.jar $(CDMW_HOME)/bin/cdmwccmcif.jar \
#	$(CDMW_HOME)/bin/cdmwccmcontainer.jar $(CDMW_HOME)/bin/cdmwccmcomponentserver.jar
#CDMW_LIB:=$(shell echo $(CDMW_CP) | sed 's/ /:/g')

# XML_LIB=$(XERCES_JAR_PATH)/xercesImpl.jar:$(XERCES_JAR_PATH)/xmlParserAPIs.jar:$(XALAN_JAR_PATH)/xalan.jar:$(XALAN_JAR_PATH)/xml-apis.jar
#CLASSPATH=$(CDMW_TUTORIAL_HOME)/prod:$(CDMW_LIB):$(ORB_LIB):$(XML_LIB)

RM=rm -f
