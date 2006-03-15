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


ret_ok=0;
ret_nok=1;


# Cdmw PullMonitorable program
pull_monitorable_exe=cdmw_test_platformfaultdetector

# Default pid directory prefix (when ProcessesIdDirectory is not specified)
default_pid_dir=/tmp/CdmwPullMonitorable_

# Name of pid file for pullMonitorable 
# This file is created into the pid directory
pull_monitorable_pid_filename="__CDMW_PULLMONITORABLE__.pid"

# print usage of this script
print_usage()
{
    echo "`basename $0`"
    echo "   This script starts the $pullMonitorable_exe." 
    echo "Usage:"
    echo "   `basename $0`  [--server --CdmwLocalisationService=PORT --PullMonitorableName=NAME --Sleep=DURATION --Request= true|false]"
    echo " "
    echo "    start the CdmwPullMonitorable in background."
    echo " "
}

pull_monitorable_args=

for arg in $@; do
   pull_monitorable_args="$pull_monitorable_args $arg"
done

name=`echo $pull_monitorable_args | cut -f3 -d' ' | cut -f2 -d'='`

pid_directory=$default_pid_dir$name

pid_file=${pid_directory}/${pull_monitorable_pid_filename}


check_running_pull_monitorable()
{
    if [ -f "$pid_file" ]; then
        pid=`cat $pid_file`
        if [ -n "`ps -edf | grep $pid | grep $pull_monitorable_exe`" ]; then
            echo "Error: pullMonitorable already started with pid $pid (as specified by ${pid_file})."
            exit $ret_nok
        else
            # pullMonitorable was killed but its pid file remains !!
            echo "Warning: old pullMonitorable was badly stopped."
            exit $ret_nok
        fi
    fi
}



# stop_pull_monitorable
stop_pull_monitorable()
{
    if [ -d $pid_directory ]; then

        for i in `ps -edfww | grep $USER | grep cdmw_test_platformfaultdetector | grep $name | awk '{print $2}'`
        do
           kill -9 $i
        done

        rm -rf $pid_directory

        echo "  ... done !"
    fi
}


start_pull_monitorable()
{
    check_running_pull_monitorable
    
    # call stop_pull_monitorable at exit
    trap 'stop_pull_monitorable' 0
    
    # ignore HUP signal
    trap '' 1
    
    # Problem with SunOS: 
    # following signal must be explicitly traped. (trap 0 is not enough !)
    if [ "`uname`" = "SunOS" ]; then
        trap 'stop_pull_monitorable' 2 15
    fi

    pull_monitorable_pid=$$

    if [ ! -d $pid_directory ]
    then    
      mkdir $pid_directory
      chmod 777 $pid_directory
    fi

    echo $pull_monitorable_pid > $pid_file

    # start pullMonitorable 
    exec $pull_monitorable_exe $pull_monitorable_args
}

start_pull_monitorable
wait

exit $ret_ok

