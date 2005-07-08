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


# BEGIN CDMW INSTALLATION DATA
# -------------------------------------------------------------------------------
# CDMW installation folder
CDMW_HOME=@config.install.dir@
# CDMW demo installation folder
CDMW_DEMO_HOME=@config.install.dir@/demos/demo_philosophers/java

XALAN_JAR_PATH=@config.XalanJAVA.lib.dir@
XERCES_JAR_PATH=@config.XercesJAVA.lib.dir@

SAXON_JAR_PATH=@config.Saxon.lib.dir@
OPENCCM_BIN_PATH=@config.OpenCCM.bin.dir@

ORBACUS_JAVA_IDL_PATH=@config.ORBacusJAVA.idl.dir@
ORBACUS_JAVA_JAR_PATH=@config.ORBacusJAVA.lib.dir@
ORBACUS_JAVA_BIN_PATH=@config.ORBacusJAVA.bin.dir@

FTP_DIR=@config.FtpJAVA.ftp.dir@

ORB=@config.ProdTests.java.orb@

ifeq (@config.ProdTests.java.orb@,JacORB)
JACORB_IDL_PATH=@config.JacORB.idl.dir@
JACORB_JAR_PATH=@config.JacORB.lib.dir@
JACORB_BIN_PATH=@config.JacORB.bin.dir@
TAO_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
endif

# -------------------------------------------------------------------------------
# END CDMW INSTALLATION DATA
# -------------------------------------------------------------------------------

