#!/bin/sh
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

#####################################################################
# CDMW Memory Cleaner - v. 0.4 - (c) RM
# Kill all running applications in PERFTEST_APP list for current user
# Usage: kill_cdmw_appl.sh [ProcessName]...
#####################################################################


KILL_LIST="cdmw_ft_ cdmw_lb_ cdmw_platform_ cdmw_naming_"
KILL_LIST="$KILL_LIST cdmw_property_ Monitor.sh Jacorb OpenCCM"
KILL_LIST="$KILL_LIST i686-pc-linux-gnu/server i686-pc-linux-gnu/client"

if test ! "$1" == ""
then # add other process to kill list
  KILL_LIST="$KILL_LIST $*"
fi

echo "=== CDMW memory cleaner ==="
for appl_running in $KILL_LIST; do
    echo "Scanning memory for $appl_running"
    # get user id from the passwd file
    USR_UID=`cat /etc/passwd | grep -w $USER | cut -d: -f3`
    APP_PID=`ps aux | grep $appl_running | egrep -w "$USER|$USR_UID"  | grep -vw grep | cut -c10-14`
    for app_pid in $APP_PID; do
        echo -n "===> Terminating "$appl_running" with PID: $app_pid"
        echo " <==="
        kill -9 $app_pid 2>&1 | >/dev/null
    done
done

if test -e ../data/CdmwPlatformMngtDaemon_conf.xml
then
  SMG_PORT=`cat ../data/CdmwPlatformMngtDaemon_conf.xml | grep "<SystemPort>" | cut -d\> -f2 | cut -d\< -f1`
  rm -rf /tmp/CdmwDaemon_$SMG_PORT 2>&1 | >/dev/null&
else
  echo Removing /tmp/Cdmw* directories....
  rm -rf /tmp/Cdmw* 2>&1 | >/dev/null&
fi
echo done!
