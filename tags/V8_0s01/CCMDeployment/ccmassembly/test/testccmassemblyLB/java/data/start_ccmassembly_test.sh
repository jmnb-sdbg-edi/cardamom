#!/bin/sh
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


# ports
IRSERV_PORT=21000


# we start the interface repository
echo Starting Interface Repository on port $IRSERV_PORT...

#case @config.CCMContainer.java.orb@ in
#
#    ORBacusJAVA)
#        irserv -OAport $IRSERV_PORT &
#        IRSERV_PID=$!
#        ;;
#
#    *)
#        echo " invalid ORB constraint...";
#        exit 1;
#        ;;
#
#esac

cdmw_start_ifr_service.sh --ifr-port $IRSERV_PORT &
IRSERV_PID=$!

sleep 4

IRFLAG_ORB="-ORBInitRef InterfaceRepository=corbaloc::localhost:$IRSERV_PORT/DefaultRepository"

echo Feeding Interface Repository...

#case @config.CCMContainer.java.orb@ in
#
#    ORBacusJAVA)        
#        irfeed ${INCLUDE_IDL_PATH}  $IRFLAG_ORB  Basic_cif.idl
#        ;;
#
#    *)
#        echo " invalid ORB constraint...";
#        exit 
#        ;;
#
#esac

cdmw_start_ifr_feed.sh ${INCLUDE_IDL_PATH} Basic.idl


echo Starting tests...
java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.assembly.ObjectRefLoaderTest $IRFLAG_ORB

_IS_LB_AVAILABLE=`echo "@config.sel.min.cscis@" | grep LoadBalancing`


if [ -n "$_IS_LB_AVAILABLE" ]; then
   # LB is available: test CCM with LB
   java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.assembly.LBAssemblyLoaderTest  $IRFLAG_ORB
   java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.assembly.LBAssemblyTest  $IRFLAG_ORB
else
   # FT (or LB) is not available: test CCM alone
   java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.assembly.AssemblyLoaderTest  $IRFLAG_ORB
   java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.assembly.AssemblyTest  $IRFLAG_ORB
fi


# we stop the Interface Repository
kill -9 $IRSERV_PID



