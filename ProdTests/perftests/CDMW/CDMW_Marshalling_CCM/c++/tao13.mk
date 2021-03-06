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


# -------------------------------------------------------------------------------
# Compilation flag for tao1.3.0 as needed by CARDAMOM
# -------------------------------------------------------------------------------
HOST_SUFFIX:=$(shell echo $(HOST_TYPE) | cut -f 3 -d"-" | tr "." "_")
CFLAGS_solaris2_8=
CFLAGS_linux= -DTAO_USE_SEQUENCE_TEMPLATES

tao=4203

CPPFLAGS_tao13= -I$(ACE_ROOT) -I$(TAO_ROOT) -I$(TAO_ROOT)/orbsvcs \
            -I$(TAO_ROOT)/tao/IFR_Client -I$(TAO_ROOT)/orbsvcs/orbsvcs -I$(TAO_ROOT)/orbsvcs/orbsvcs/IFRService \
            -Dtao=$(tao) -DCDMW_ORB_VDR=$(tao) -DCDMW_ORB_VER=13  \
            -DACE_HAS_EXCEPTIONS -D_POSIX_THREADS -D_POSIX_THREAD_SAFE_FUNCTIONS -D_REENTRANT -DACE_HAS_AIO_CALLS -DTAO_HAS_VALUETYPE $(CFLAGS_$(HOST_SUFFIX))
CXXFLAGS_tao13=
IDLFLAGS_tao13= -I$(TAO_ROOT)/TAO_IDL/idl_specs -I$(TAO_ROOT)/tao -I$(TAO_ROOT)/orbsvcs -I$(TAO_ROOT)/orbsvcs/orbsvcs -I$(TAO_ROOT)/tao/IFR_Client
LDFLAGS_tao13:= -L$(ACE_ROOT)/ace 
LIBS_tao13:= -lTAO_CosEvent -lTAO_CosProperty -lTAO -lTAO_PortableServer -lTAO_CosNaming -lTAO_Strategies -lTAO_Messaging -lTAO_RTCORBA -lACE -lTAO_IFR_Client
IDL2CXX_tao13=ACE_ROOT=$(ACE_ROOT) TAO_ROOT=$(TAO_ROOT) $(TAO_ROOT)/TAO_IDL/tao_idl -DTAO \
		    -Ge 0 -Gv -Sc -Sp


# -------------------------------------------------------------------------------
