#!/bin/sh
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


TESTS_FILE="CdmwValidationTests.output"

rm -f core
rm -f $TESTS_FILE


# set Splice environment if defined
SPLICE_CONFIG_FILE=splice/release.com

if test -f "$SPLICE_CONFIG_FILE"
then
    . $SPLICE_CONFIG_FILE
fi

case @config.SystemMngt.cpp.orb@ in
	ORBacusCPP)

	# start naming service for time service
	NAME_PID=
	TIME_PID=
	
	echo "starting the Name Server ..."
	NAMING_PORT=2001
	NAMESERVIOR=nameService.ior
	rm -f $NAMESERVIOR
	nameserv --ior -OAport $NAMING_PORT > $NAMESERVIOR &
	NAME_PID=$!
	
	stop()
	{
	if [ $NAME_PID != "" ]; then
		echo "killing the Name Server ..."
		kill -9 $NAME_PID
	fi
        
	if [ $TIME_PID != "" ]; then
		echo "killing the Time Server ..."
		kill -9 $TIME_PID
	fi
        
	exit 1
	}
	
	trap 'stop;' 1 2 15
	
	# wait ior creation
	until [ -s $NAMESERVIOR ]; do
	sleep 1
	done
	
	NAMING_URL=corbaloc::1.2@localhost:$NAMING_PORT/NameService
	
	echo "starting the Time Server ..."
	
	start_time_service.sh --naming-url $NAMING_URL &
	TIME_PID=$!
	
	sleep 4
	
	
		;;
		
    TAO|TAOOpenFusion)
		;;

	*)
		echo " invalid ORB constraint...";
		exit 1;
		;;
esac


echo "starting the Platform Daemon ..."

trap 'echo "Signal trapped, stop command send to platform daemon.";cdmw_platform_daemon.sh stop; exit' 2

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml | tee $TESTS_FILE

