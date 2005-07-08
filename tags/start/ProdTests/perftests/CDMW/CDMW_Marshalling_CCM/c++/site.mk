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


# CDMW installation folder

# REM: a try to use /net/hostX don't work because
# the scripts in install directory do not have this /net/hostX.
# But you can use symbolic link at the remote host

CDMW_PERF_HOME=$(PWD)

# Xalan JAVA
XALAN_JAR_PATH=$(XALAN_ROOT)/bin

# Xerces C++
XERCES_INC_PATH=$(XERCES_ROOT)
XERCES_LIB_PATH=$(XERCES_ROOT)/lib

# Xerces JAVA
XERCES_JAR_PATH=$(XERCES_JAR)

# Saxon
SAXON_JAR_PATH=$(SAXON_JAR)

# OpenCCM
OPENCCM_BIN_PATH=$(OPENCCM_ROOT)/bin
OPENCCM_IDL_PATH=$(OPENCCM_ROOT)/idl

# Zip
ZIP_BIN_PATH=$(ZIP_ROOT)/bin

# C++ ORB
ifeq (TAO,TAO)
ORB_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_LIB_PATH=$(ACE_ROOT)/ace 
ORB_CPP=tao14-OF
ORB_CPP_NAME=tao
endif

ifeq (ORBacusJAVA,JacORB)
JACORB_IDL_PATH=@config.JacORB.idl.dir@
JACORB_JAR_PATH=@config.JacORB.lib.dir@
JACORB_BIN_PATH=@config.JacORB.bin.dir@
TAO_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_JAVA=JacORB20
endif

ORB=$(ORB_CPP)
	 
