#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI. All rights reserved.
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
CDMW_PERF_HOME=$(PWD)
XERCES_INC_PATH=$(XERCES_ROOT)
XERCES_LIB_PATH=$(XERCES_ROOT)/lib

ifeq (TAO,TAO)
ORB_IDL_PATH=$(ACE_ROOT)/TAO/orbsvcs/orbsvcs
ORB_LIB_PATH=$(ACE_ROOT)/ace 
ORB=tao14-OF
endif
ifeq (TAO,ORBacusCPP)
ORBACUS_IDL_PATH=@config.ORBacusCPP.idl.dir@
ORBACUS_BIN_PATH=@config.ORBacusCPP.bin.dir@
ORBACUS_LIB_PATH=@config.ORBacusCPP.lib.dir@
ORBACUS_INC_PATH=@config.ORBacusCPP.include.dir@
ORB=ORBacus4
endif

# -------------------------------------------------------------------------------
# END CDMW INSTALLATION DATA
# -------------------------------------------------------------------------------

