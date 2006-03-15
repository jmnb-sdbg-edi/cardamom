# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================


# CDMW installation folder
CDMW_HOME=@config.install.dir@

# Xalan JAVA
XALAN_JAR_PATH=@config.XalanJAVA.lib.dir@

# Xerces C++
XERCES_INC_PATH=@config.XercesCPP.install.dir@
XERCES_LIB_PATH=@config.XercesCPP.lib.dir@

# Xerces JAVA
XERCES_JAR_PATH=@config.XercesJAVA.lib.dir@

# Saxon
SAXON_JAR_PATH=@config.Saxon.lib.dir@

# OpenCCM
OPENCCM_BIN_PATH=@config.OpenCCM.bin.dir@

# FTP home directory
FTP_DIR=@config.Ftp.homedir@

# Splice
SPLICE_INC_PATH=@config.Splice.include.dir@
SPLICE_LIB_PATH=@config.Splice.lib.dir@
SPLICE_BIN_PATH=@config.Splice.bin.dir@


# C++ ORB
ifeq (@config.cpp.orb@,TAO)
ORB_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_LIB_PATH=$(ACE_ROOT)/ace 
ORB_CPP=tao14-OF
ORB_CPP_NAME=tao
endif
ifeq (@config.ProdTests.cpp.orb@,TAO)
ORB_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_LIB_PATH=$(ACE_ROOT)/ace 
ORB=tao13
endif
ifeq (@config.ProdTests.cpp.orb@,TAOOpenFusion)
ORB_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_LIB_PATH=$(ACE_ROOT)/ace 
ORB=tao14-OF
endif
ifeq (@config.ProdTests.cpp.orb@,ORBacusCPP)
ORBACUS_IDL_PATH=@config.ORBacusCPP.idl.dir@
ORBACUS_BIN_PATH=@config.ORBacusCPP.bin.dir@
ORBACUS_LIB_PATH=@config.ORBacusCPP.lib.dir@
ORBACUS_INC_PATH=@config.ORBacusCPP.include.dir@
ORB=ORBacus4
endif

# JAVA ORB
ifeq (@config.java.orb@,ORBacusJAVA)
ORBACUS_JAVA_IDL_PATH=@config.ORBacusJAVA.idl.dir@
ORBACUS_JAVA_JAR_PATH=@config.ORBacusJAVA.lib.dir@
ORBACUS_JAVA_BIN_PATH=@config.ORBacusJAVA.bin.dir@
ORB_JAVA=ORBacus410
endif
ifeq (@config.java.orb@,JacORB)
JACORB_IDL_PATH=@config.JacORB.idl.dir@
JACORB_JAR_PATH=@config.JacORB.lib.dir@
JACORB_BIN_PATH=@config.JacORB.bin.dir@
TAO_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_JAVA=JacORB20
endif
