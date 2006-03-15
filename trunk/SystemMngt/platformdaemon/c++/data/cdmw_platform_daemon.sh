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


# Cdmw Daemon program
daemon_exe=@CDMW_PLATFORM_DAEMON_PATH@/cdmw_platform_daemon

# Script to monitor the daemon 
daemon_monitoring_exe=@CDMW_PLATFORM_DAEMON_SCRIPTS_PATH@/cdmw_platform_daemon_monitoring.sh

# Script to kill all processes created by daemon
kill_all_pids_exe=@CDMW_PLATFORM_SCRIPTS_PATH@/cdmw_platform_kill_all_pids.sh

# Default pid directory prefix (when ProcessesIdDirectory is not specified)
# NOTE: this is also defined into platformdaemon/c++/src/ConfigurationMngr.cpp
#       and into the awk program below.
default_pid_dir=/tmp/CdmwDaemon_

# Default Daemon port (when daemon is started without any option)
# NOTE: this is also defined into export/idl/CdmwPlatformMngtCommon.idl
default_system_port=21869

# Name of pid file for daemon (only used with start and stop commands)
# This file is created into the pid directory
daemon_pid_filename="__CDMW_DAEMON__.pid"

# Foo option used by cdmw_platform_daemon to check 
# if it is launched by this script 
started_by_script_checking_option="--isStartedByScript"


# print usage of this script
print_usage()
{
    echo "`basename $0`"
    echo "   This script starts the $daemon_exe, and wait its death." 
    echo "   Then, it kills all agents launched by this daemon, and all processes launched by those agents."
    echo "Usage:"
    echo "   `basename $0`  [--CdmwLocalisationService=PORT | --CdmwXMLFile=XMLConfigFile [--validateXML] ] [start | stop]"
    echo " "
    echo "   start      : start the CdmwDaemon in background."
    echo "   stop       : stop the CdmwDaemon. This command should be used with the same options than start !"
    echo "   no command : start the CdmwDaemon in foreground."
    echo " "
}



# return the pidDirectory according to the xml_conf_file:
#   - value of <ProcessesIdDirectory> (if specified)
#   - $default_pid_dir$port with port = value of <SystemPort>
getPidDir()
{
    if [ -z "$1" ]; then
        echo "Usage: getPidDir <xml_conf_file>"
        return $ret_nok
    fi

    # "grep ." is to ignore blank lines.
    cat $1 | grep . | awk '
        # BEGIN OF AWK PROGRAM
        BEGIN {RS=""}
        {
            text=$0
            
            # Supress all comments
            while ((i = index(text, "<!--")) != 0) {
                if (i == 1) {
                    before_comment = ""
                } else {
                    before_comment = substr(text, 1, i-1)
                }
                
                if ((j = index(text, "-->")) == 0) {
                    # Error message could not be directed to "/dev/stderr".
                    # This doesn t exist on AIX !!
                    print "Error: \"<!--\" without corresponding \"-->\" !!"
                    exit 1
                }
                
                if (j < i) {
                    print "Error: their is an isolated \"-->\" !! "
                    exit 1
                }
                
                after_comment = substr(text, j+3)
                
                text = before_comment after_comment
            }
            
            # Get value of <ProcessesIdDirectory>
            if ((i = index(text, "<ProcessesIdDirectory>")) != 0) {
                j = index(text, "</ProcessesIdDirectory>")
                xml_conf_file = substr(text, i+22, j-i-22)
                print xml_conf_file
            
            # Get value of <SystemPort>    
            } else if ((i = index(text, "<SystemPort>")) != 0) {
                j = index(text, "</SystemPort>")
                system_port = substr(text, i+12, j-i-12)
                print "/tmp/CdmwDaemon_"system_port
            
            } else {
                # At least <SystemPort> must be specified
                print "Error: <SystemPort> is requiered !!"
                exit 1
            }
            
            
        }
        # END OF AWK PROGRAM
    '
    
    return $?;
}


arg= 
port_value= 
xml_conf_file=
daemon_args=
command=

for arg in $@; do
    case $arg in
        --help)
            print_usage
            exit $ret_ok
            ;;
        
        --CdmwLocalisationService=?*)
            port_value=`echo $arg | sed -e "s%--CdmwLocalisationService=%%"`
            daemon_args="$daemon_args $arg"
            ;;
            
        --CdmwXMLFile=?*)
            xml_conf_file=`echo $arg | sed -e "s%--CdmwXMLFile=%%"`
            daemon_args="$daemon_args $arg"
            ;;
            
        --CdmwORBThreads=?*)
            daemon_args="$daemon_args $arg"
            ;;

        --validateXML)
            # ignore --validateXML option
            daemon_args="$daemon_args $arg"
            ;;

        --FaultManagerRegistration=?*)
            # ignore this option
            daemon_args="$daemon_args $arg"
            ;;
        
        start)
            if [ -n "$command" ]; then
                echo "Error: commands start and stop used together."
                print_usage
                exit $ret_nok
            fi
            command=start
            ;;
            
        stop)
            if [ -n "$command" ]; then
                echo "Error: commands start and stop used together."
                print_usage
                exit $ret_nok
            fi
            command=stop
            ;;
            
        *)
            echo "Error: unknown option '$arg'"
            print_usage
            exit $ret_ok
            ;;
    esac
done


if [ -n "$xml_conf_file" -a -n "$port_value" ]; then
    echo "Error: --CdmwLocalisationService and --CdmwXMLFile should not be used together."
    echo ""
    print_usage
fi


if [ -z "$xml_conf_file" -a -z "$port_value" ]; then
    # No option. pid_directory is "$default_pid_dir$default_system_port"
    pid_directory=$default_pid_dir$default_system_port
fi

if [ -n "$port_value" ]; then
    # --CdmwLocalisationService is used
    pid_directory=$default_pid_dir$port_value
fi

if [ -n "$xml_conf_file" ]; then
    if [ ! -f $xml_conf_file ]; then
        echo "Error: Configuration file '$xml_conf_file' not found."
        exit $ret_nok
    fi

    # --CdmwXMLFile is used
    pid_directory=`getPidDir $xml_conf_file`
    if [ $? != $ret_ok ]; then
        # if getPidDir() exit with error, pid_directory contains the error message.
        # print error message.
        echo $pid_directory
        exit $ret_nok
    fi
fi

pid_file=${pid_directory}/${daemon_pid_filename}


check_running_daemon()
{
    if [ -f "$pid_file" ]; then
        pid=`cat $pid_file`
        if [ -n "`ps -edf | grep $pid | grep $daemon_exe`" ]; then
            echo "Error: daemon already started with pid $pid (as specified by ${pid_file})."
            exit $ret_nok
        else
            # daemon was killed but its pid file remains !!
            echo "Warning: old daemon was badly stopped."
            echo "To kill all processes launched by this old daemon, run:"
            echo "   $kill_all_pids_exe $pid_directory"
            echo "Then, you can restart a daemon."
            exit $ret_nok
        fi
    fi
}



# stop_daemon operation
stop_daemon()
{
    if [ -d $pid_directory ]; then
        echo "Kill daemon and all processes it launched"
        $kill_all_pids_exe $pid_directory
        echo "  ... done !"
    fi
}


start_daemon()
{
    check_running_daemon
    
    # call stop_daemon at exit
    trap 'stop_daemon' 0
    
    # ignore HUP signal
    trap '' 1
    
    # Problem with SunOS: 
    # following signal must be explicitly traped. (trap 0 is not enough !)
    if [ "`uname`" = "SunOS" ]; then
        trap 'stop_daemon' 2 15
    fi

    $daemon_monitoring_exe $kill_all_pids_exe $pid_directory &
    
    daemon_pid=$$
    
    mkdir $pid_directory
    chmod 777 $pid_directory
    echo $daemon_pid > $pid_file

    # start daemon
    exec $daemon_exe $daemon_args $started_by_script_checking_option
}






case $command in
    '')
        start_daemon
        wait
        ;;
    
    start)
        $0 $daemon_args &
        ;;
    
    stop)
        if [ ! -f $pid_file ]; then
            echo "Daemon is not running."
            if [ -z "$daemon_args" ]; then
                echo "Warning: You don't give any option." 
                echo "         If you started daemon with some options, you should give the same options to stop it."
            fi
        else
            stop_daemon
        fi
        ;;
    
    *)
        echo "Error: unknown command \"${command}\""
        print_usage
        exit $ret_nok
        ;;
    
esac


exit $ret_ok


