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


# we start the interface repository
echo Starting Interface Repository on port $IRSERV_PORT...
case @config.Event.java.orb@ in
    ORBacusJAVA)
        irserv   -ORBconfig ./obtest.conf ${INCLUDE_IDL_PATH}  Echo.idl &
        IRSERV_PID=$!
        ;;

    *)
        echo " invalid ORB constraint...";
        exit 1;
        ;;
esac

sleep 4


echo Starting tests...
java -Xbootclasspath/p:${BOOTCLASSPATH} cdmw.event.test.MainTest


# we stop the Interface Repository
kill -9 $IRSERV_PID




