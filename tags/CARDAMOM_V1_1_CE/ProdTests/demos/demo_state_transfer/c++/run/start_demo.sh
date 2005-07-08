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

if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi


if [ "$1" != "" ]
then

    if [ "$1" != "--PointToPoint" ]
    then
        PROC_NAME=`basename $0`
        $echo "Usage: $PROC_NAME [--PointToPoint]"
        $echo "   $PROC_NAME                : Start demo with point-to-point State Transfer."
        $echo "   $PROC_NAME --Multicast    : Start demo with multicast State Transfer."
        $echo ""
        exit 1
    fi
fi

PWD=`pwd`
DEMO_DIR=`echo $PWD | sed s%/run%%g`
SITE_CONFIG=../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../configure.sh' and restart $0 "
    exit 1
fi

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME= |cut -d= -f2`"
if [ "$CDMW_HOME" = "" ]
then
    $echo "CDMW_HOME is not set into $SITE_CONFIG"
    exit 1
fi

LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH


# "hostname" used for the FaultManagerRegistration because i've heard
# of problems with "localhostname"
HOSTNAME=`hostname`
# Check HOSTNAME2 is set
HOSTNAME2="`echo $HOSTNAME2`"
if [ "$HOSTNAME2" = "" ]
then
    $echo "HOSTNAME2 is not set into $SITE_CONFIG"
    exit 1
fi
# Check HOSTNAME3 is set
HOSTNAME3="`echo $HOSTNAME3`"
if [ "$HOSTNAME3" = "" ]
then
    $echo "HOSTNAME3 is not set into $SITE_CONFIG"
    exit 1
fi

# XML scenario file
if [ "$1" = "--Multicast" ]
then
    $echo "The demo will use multicast state transfer..."
    SCENARIO_FILE=../data/ScenarioMulticast.xml
else 
    $echo "The demo will use point-to-point state transfer..."
    SCENARIO_FILE=../data/ScenarioPointToPoint.xml
fi


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

TERM_PATH=`which $TERM`

INIT_TIMEOUT=20
FT_MANAGER_TIMEOUT=20
EXEC_TIMEOUT=160
CLEANUP_TIMEOUT=5


$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="

# 0a) Check Cardamom & demo installation on host2

CDMW_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $CDMW_HOME/bin/cdmw_platform_daemon 2>/dev/null`
if [ "$CDMW_ALREADY_INSTALLED" = "" ]
then
    $echo "CARDAMOM not installed on $HOSTNAME2 !"
    $echo " (it should be installed into $CDMW_HOME directory)"
    exit 1
fi

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $DEMO_DIR/$HOST_TYPE/datastore 2>/dev/null`
if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
    $echo "This demo is not installed on $HOSTNAME2 !"
    $echo " (it should be installed into $DEMO_DIR directory)"
    exit 1
fi

# 0b) Check Cardamom & demo installation on host3

CDMW_ALREADY_INSTALLED=`rsh $HOSTNAME3 ls $CDMW_HOME/bin/cdmw_platform_daemon 2>/dev/null`
if [ "$CDMW_ALREADY_INSTALLED" = "" ]
then
    $echo "CARDAMOM not installed on $HOSTNAME3 !"
    $echo " (it should be installed into $CDMW_HOME directory)"
    exit 1
fi

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME3 ls $DEMO_DIR/$HOST_TYPE/datastore 2>/dev/null`
if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
    $echo "This demo is not installed on $HOSTNAME3 !"
    $echo " (it should be installed into $DEMO_DIR directory)"
    exit 1
fi



# 1) Start daemon 
$echo Starting the Platform Management Daemon...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"
TERM_COMMAND1="$TERM -sb -sl 5000 -geometry 90x30+0+0 -title ${HOSTNAME}_platform_daemon"
$TERM_COMMAND1 -e $DAEMON_COMMAND &


DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DEMO_DIR/data/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND2="$TERM -sb -sl 5000 -geometry 90x30-0+0 -title ${HOSTNAME2}_platform_daemon -display $DISPLAY"
rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_COMMAND2 -e $DAEMON_COMMAND2)" &

DAEMON_COMMAND3="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DEMO_DIR/data/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND3="$TERM -sb -sl 5000 -geometry 90x30-0-0 -title ${HOSTNAME3}_platform_daemon -display $DISPLAY"
rsh $HOSTNAME3 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_COMMAND3 -e $DAEMON_COMMAND3)" &

trap '$DAEMON_COMMAND stop; exit' 2
#rsh $HOSTNAME2 trap '$DAEMON_COMMAND2 stop; exit' 2

sleep $INIT_TIMEOUT

# 1b) Start FT Manager
$echo Starting the FT Manager...
$CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=$DEMO_DIR/data/CdmwFaultToleranceManager_conf.xml &
FT_MANAGER_PID=$!
sleep $FT_MANAGER_TIMEOUT

# 2) Start Platform Management Supervision
$echo Starting the Platform Management Supervision...
$CDMW_HOME/bin/cdmw_platform_supervision --CdmwLocalisationService=21888 --FaultManagerRegistration=corbaloc::$HOSTNAME:4608/fault_manager --RequestDurationTime=20000000 --creation-timeout=20000 &
SUPERVISION_PID=$!
sleep $INIT_TIMEOUT

# 3) Define system
$echo Defining the System '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21888/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

# 4) Start_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21888/CdmwPlatformMngtSupervision --sys-start
if [ $? -eq "0" ];
then
    #$echo "Waiting a while for the execution of the scenario...\c"
    #sleep $EXEC_TIMEOUT
    $echo "Hit a key when the scenario is ended...\c"
    read FOO
    $echo done.
fi

# 5) Get a snapshot of the system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21888/CdmwPlatformMngtSupervision --sys-snapshot

# 6) stop_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21888/CdmwPlatformMngtSupervision --sys-stop
sleep $CLEANUP_TIMEOUT

$echo "=============================================================="
$echo "Hit a key when ready to stop the CDMW Daemon and Supervision."
read FOO
$echo "=============================================================="

$DAEMON_COMMAND stop
rsh $HOSTNAME2 $DAEMON_COMMAND2 stop
rsh $HOSTNAME3 $DAEMON_COMMAND3 stop
kill -9 $SUPERVISION_PID
kill -9 $FT_MANAGER_PID
$echo "done."

rm -f *.ior
rm -f *.IOR

