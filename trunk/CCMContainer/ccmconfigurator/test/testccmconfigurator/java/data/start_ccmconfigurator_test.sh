#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES 
# and SELEX-SI. 
# 
# It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
# All rights reserved.
# 
# CARDAMOM is free software; you can redistribute it and/or modify it under 
# the terms of the GNU Library General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your 
# option) any later version. 
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
# License for more details. 
# 
# You should have received a copy of the GNU Library General 
# Public License along with CARDAMOM; see the file COPYING. If not, write to 
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================


# ports
XMLSERV_PORT=5060
IRSERV_PORT=21000

echo Starting XML Server on port $XMLSERV_PORT...
java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.xmlutils.XMLServer --CdmwLocalisationService=$XMLSERV_PORT --doctypedir=. --validate &
XML_SERVER_PID=$!
sleep 4

# we start the interface repository
echo Starting Interface Repository on port $IRSERV_PORT...

#case @config.CCMContainer.java.orb@ in
#
#	ORBacusJAVA)
#		irserv -OAport $IRSERV_PORT &
#		IRSERV_PID=$!
#		;;
#
#   JacORB)
#		IFR_Service -ORBEndpoint iiop://localhost:$IRSERV_PORT &
#		;;
#	*)
#		echo " invalid ORB constraint...";
#		exit 1;
#		;;
#esac


cdmw_start_ifr_service.sh --ifr-port $IRSERV_PORT &
IRSERV_PID=$!

sleep 4


echo Feeding Interface Repository...

#case @config.CCMContainer.java.orb@ in
#
#	ORBacusJAVA)
#		irfeed   ${INCLUDE_IDL_PATH} -ORBconfig ./obtest.conf  Cdmw_hello.idl
#		;;
#
#	JacORB)
#		tao_ifr -I@config.JacORB.install.dir@/idl/omg/ ${INCLUDE_IDL_PATH} -ORBInitRef InterfaceRepository=corbaloc:iiop:localhost:$IRSERV_PORT/InterfaceRepository Cdmw_hello.idl
#		;;
#
#   *)
#		echo " invalid ORB constraint...";
#		exit 1;
#		;;
#
#esac


cdmw_start_ifr_feed.sh ${INCLUDE_IDL_PATH} hello.idl


echo Starting tests...
java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.ccm.configurator.test.MainTest


# we stop the Interface Repository
kill -9 $IRSERV_PID

# we stop the XML server
kill -9 $XML_SERVER_PID



