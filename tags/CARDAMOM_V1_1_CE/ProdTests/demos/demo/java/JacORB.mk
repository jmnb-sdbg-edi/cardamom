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

JAVA_IDL_COMPILER=$(JACORB_BIN_PATH)/idljaco
JIDL_CLT=$(JAVA_IDL_COMPILER) 
JIDL_SVR=$(JAVA_IDL_COMPILER)

JIDL_FLAGS_CLT=-noskel
JIDL_FLAGS_SVR=
JIDL_FLAGS_TIE=

ORB_JAR_PATH:=$(JACORB_JAR_PATH)
ORB_JARS:= jacorb.jar idl.jar antlr-2.7.2.jar avalon-framework-4.1.5.jar  concurrent-1.3.2.jar logkit-1.2.jar wrapper-3.0.3.jar
ORB_CP=$(foreach JAR,$(ORB_JARS),$(ORB_JAR_PATH)/$(JAR) )
ORB_LIB:=$(shell echo $(ORB_CP) | sed 's/ /:/g')

JIDL_INCLUDE_DIR_OPTION:=-I
JIDLFLAGS_ORB= -I$(JACORB_IDL_PATH) -I$(JACORB_IDL_PATH)/omg
JIDL_INCLUDE_DIR:= -I$(CDMW_DEMO_HOME)/idl -I$(CDMW_HOME)/idl
JIDLFLAGS=$(JIDL_INCLUDE_DIR) $(JIDLFLAGS_ORB) \
	-i2jpackagefile $(CDMW_DEMO_HOME)/package.txt -d $(CDMW_DEMO_HOME)/prod/JacORB
