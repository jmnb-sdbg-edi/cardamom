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

SITE_CONFIG=../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../configure.sh' and restart $0 "
    exit 1
fi

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"
LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH

# "hostname" used for the FaultManagerRegistration because i've heard
# of problems with "localhostname"
HOSTNAME=`hostname`
# Check HOSTNAME2 is set
if [ "$HOSTNAME2" = "" ]
then
    $echo "Export HOSTNAME2 to set the other host used by this demo. Then, run 'make data' in parent directory restart $0"
	 rm ../data/Scenario.xml
    exit 1
fi

# XML scenario file
SCENARIO_FILE=../data/Scenario.xml

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

INIT_TIMEOUT=35
FT_MANAGER_TIMEOUT=30
EXEC_TIMEOUT=160
CLEANUP_TIMEOUT=5


$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

# 0) Install exe on HOSNAME2
BUILD_TARGET=`$CDMW_HOME/bin/config.guess`
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $CDMW_HOME/demos/demo_ft/c++/$BUILD_TARGET 2>/dev/null` 

if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
   $echo Installing demo on $HOSTNAME2
	rsh $HOSTNAME2 mkdir -p $CDMW_HOME
	$echo "  => Copying $CDMW_HOME/bin"
   rcp -r $CDMW_HOME/bin $HOSTNAME2:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/lib"
   rcp -r $CDMW_HOME/lib $HOSTNAME2:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/share"
   rcp -r $CDMW_HOME/share $HOSTNAME2:$CDMW_HOME
   rsh $HOSTNAME2 mkdir -p $CDMW_HOME/demos/demo_ft/c++
	$echo "  => Copying $CDMW_HOME/demos/demo_ft/c++/site.mk"
   rcp $CDMW_HOME/demos/demo_ft/c++/site.mk $HOSTNAME2:$CDMW_HOME/demos/demo_ft/c++
   rsh $HOSTNAME2 mkdir $CDMW_HOME/demos/demo_ft/c++/generated
	rcp $CDMW_HOME/demos/demo_ft/c++/generated/Server_Desc.dtd $HOSTNAME2:$CDMW_HOME/demos/demo_ft/c++/generated
	$echo "  => Copying $CDMW_HOME/demos/demo_ft/c++/data"
   rcp -r $CDMW_HOME/demos/demo_ft/c++/data $HOSTNAME2:$CDMW_HOME/demos/demo_ft/c++
	$echo "  => Copying $CDMW_HOME/demos/demo_ft/c++/run"
   rcp -r $CDMW_HOME/demos/demo_ft/c++/run $HOSTNAME2:$CDMW_HOME/demos/demo_ft/c++

   rsh $HOSTNAME2 mkdir -p $CDMW_HOME/demos/demo_ft/c++/$BUILD_TARGET
	$echo "  => Copying $CDMW_HOME/demos/demo_ft/c++/$BUILD_TARGET"
   rcp -r $CDMW_HOME/demos/demo_ft/c++/$BUILD_TARGET $HOSTNAME2:$CDMW_HOME/demos/demo_ft/c++
   if [ -d  $CDMW_HOME/../tools ]
   then
	$echo "  => Copying $CDMW_HOME/../tools"
	rcp -r $CDMW_HOME/../tools $HOSTNAME2:$CDMW_HOME/..
   fi
else
   $echo Demo already installed on $HOSTNAME2
fi



# 1) Start daemon 
$echo Starting the Platform Management Daemon...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"
$TERM -ls -sb -sl 5000 -e $DAEMON_COMMAND &

DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$CDMW_HOME/demos/demo_ft/c++/data/CdmwPlatformMngtDaemon_conf.xml"

rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_PATH -display $DISPLAY -sb -sl 5000 -e $DAEMON_COMMAND2)" &

trap '$DAEMON_COMMAND stop; exit' 2
#rsh $HOSTNAME2 trap '$DAEMON_COMMAND2 stop; exit' 2

sleep $INIT_TIMEOUT

# 1b) Start FT Manager
$echo Starting the FT Manager...
$CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=$CDMW_HOME/demos/demo_ft/c++/data/CdmwFaultToleranceManager_conf.xml &
FT_MANAGER_PID=$!
sleep $FT_MANAGER_TIMEOUT

# 2) Start Platform Management Supervision
$echo Starting the Platform Management Supervision...
$CDMW_HOME/bin/cdmw_platform_supervision --CdmwLocalisationService=21885 --FaultManagerRegistration=corbaloc::$HOSTNAME:4605/fault_manager --RequestDurationTime=20000000 --creation-timeout=20000 &
SUPERVISION_PID=$!
sleep $INIT_TIMEOUT

# 3) Define system
$echo Defining the System '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21885/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

# 4) Start_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21885/CdmwPlatformMngtSupervision --sys-start
if [ $? -eq "0" ];
then
    $echo "Waiting a while for the execution of the scenario...\c"
    sleep $EXEC_TIMEOUT
    $echo done.
fi

# 5) Get a snapshot of the system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21885/CdmwPlatformMngtSupervision --sys-snapshot

# 6) stop_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21885/CdmwPlatformMngtSupervision --sys-stop
sleep $CLEANUP_TIMEOUT

$echo "=============================================================="
$echo "Hit a key when ready to stop the CDMW Daemon and Supervision."
read FOO
$echo "=============================================================="

$DAEMON_COMMAND stop
rsh $HOSTNAME2 $DAEMON_COMMAND2 stop
kill -9 $SUPERVISION_PID
kill -9 $FT_MANAGER_PID
$echo "done."

rm -f *.ior
rm -f *.IOR

