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
#


if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi

SITE_CONFIG=../../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../../configure.sh' and restart $0 "
    exit 1
fi

# CARDAMOM installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"
LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH

# XML scenario file
SCENARIO_FILE=../data/SystemDefinition.xml

if test ! -f "$SCENARIO_FILE"
then
    $echo "$SCENARIO_FILE file is missing. Run 'make data' in parent directory restart $0 "
    exit 1
fi

# XML Platform Management Daemon configuration file
DAEMON_CONF_FILE=../data/CdmwPlatformMngtDaemon_conf.xml

if test ! -f "$DAEMON_CONF_FILE"
then
    $echo "$DAEMON_CONF_FILE file is missing. Run 'make data' in parent directory restart $0 "
    exit 1
fi


if test ! -x `which $TERM`
then
    TERM=xterm
fi


START_TIMEOUT=30
INIT_TIMEOUT=10
CLEANUP_TIMEOUT=9

FT_MANAGER_TIMEOUT=15

ADMIN_ERROR="no"


$echo "========================================================="
$echo "=                  Cdmw.Tutorial.                      ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

# 1) Start daemon 
$echo Starting the Platform Management Daemon...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"
#$TERM -ls -sb -sl 1000 -e 
$DAEMON_COMMAND &

trap '$DAEMON_COMMAND stop; exit' 2

sleep $INIT_TIMEOUT

# 1a) Start FT Manager
if test -x "$CDMW_HOME/bin/cdmw_ft_manager"
then
    $echo Starting the FT Manager...
    $TERM -ls -sb -sl 1000 -e $CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=../data/CdmwFaultToleranceManager_conf.xml --groupConf=../data/CdmwFTSystemMngtGroupCreator_conf.xml &
    FT_MANAGER_PID=$!
    sleep $FT_MANAGER_TIMEOUT
fi

# 2) Start Platform Management Supervision
$CDMW_HOME/bin/cdmw_platform_supervision_starter --CdmwXMLFile=../data/CdmwPlatformMngtSystemStart.xml --validate
sleep $INIT_TIMEOUT


# 3) Start Platform Management Supervision Observer
$echo Starting the Platform Management Supervision Observer...
$TERM -ls -sb -sl 1000 -e $CDMW_HOME/bin/cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --observer-name=observer &
SUPERVISION_OBSERV_PID=$!
sleep $INIT_TIMEOUT


# 4) Define system
$echo Defining the System with '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE
sleep $INIT_TIMEOUT

# 5) Start_system
$echo Starting the System
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-start
if [ $? -eq "0" ];
then
    $echo "Waiting a while for the execution of the scenario...\c"
    sleep $START_TIMEOUT
    $echo done.
fi

# 6) Get a snapshot of the system
$echo Get a snapshot of the System
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-snapshot


# 7) Execute deployment
$echo
$echo ============================
$echo Execute deployment commands
$echo ============================
$echo
$TERM -ls -sb -sl 1000 -e ./startDeploymentTool.sh




$echo "=============================================================="
$echo "Hit a key when ready to stop the System.                      "
read FOO
$echo "=============================================================="


# 8) stop_system
$echo Stopping the System
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-stop
sleep $CLEANUP_TIMEOUT

$echo "=============================================================="
$echo "Hit a key when ready to kill the CARDAMOM Daemon and Supervision."
read FOO
$echo "=============================================================="

$DAEMON_COMMAND stop
kill -9 $SUPERVISION_OBSERV_PID $SUPERVISION_PID
if test -x "$CDMW_HOME/bin/cdmw_ft_manager"
then
    kill -9 $FT_MANAGER_PID
fi
$echo "done."

rm -f *.ior
rm -f *.IOR
