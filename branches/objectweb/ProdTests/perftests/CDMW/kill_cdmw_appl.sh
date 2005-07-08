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

#####################################################################
# Kill all running applications in PERFTEST_APP list for current user
#####################################################################

PERFTEST_APP="cdmw_platform_agent cdmw_naming_and_repository java "
PERFTEST_APP="$PERFTEST_APP cdmw_platform_supervision cdmw_platform_daemon Monitor.sh"
PERFTEST_APP="$PERFTEST_APP i686-pc-linux-gnu/server i686-pc-linux-gnu/client"

for appl_running in $PERFTEST_APP; do
	echo Scanning memory for $appl_running
	APP_PID=`ps aux | grep $appl_running | grep $USER  | grep -v grep | cut -c10-14`
	for app_pid in $APP_PID; do
		echo Terminating $appl_running with PID: $app_pid
		kill -9 $app_pid
	done
done

echo Removing /tmp/Cdmw* directories....
rm -rf /tmp/Cdmw*
echo done!
