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

if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi

SITE_CONFIG=../../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../configure.sh' and restart $0 "
    exit 1
fi

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"
CDMW_TUTORIAL_HOME="`cat $SITE_CONFIG | grep CDMW_TUTORIAL_HOME |cut -d= -f2`"
LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
$echo `echo $LD_LIBRARY_PATH` 
PATH=$CDMW_HOME/bin:$PATH
export PATH

# "hostname" used for the FaultManagerRegistration because i've heard
# of problems with "localhostname"
HOSTNAME=`hostname`
# Check HOSTNAME2 is set
if [ "$HOSTNAME2" = "" ]
then
    $echo "Export HOSTNAME2 to set the other host used by this demo. Then, run 'make data' in parent directory restart $0"
	 rm ../data/ScenarioTimers.xml
    exit 1
fi

# XML scenario file
SCENARIO_FILE=../data/ScenarioTimers.xml

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

INIT_TIMEOUT=5
FT_MANAGER_TIMEOUT=30
EXEC_TIMEOUT=160
CLEANUP_TIMEOUT=5

if [ "$HOSTNAME1X" = "" ]
then
    $echo "Export HOSTNAME1X to set the DISPLAY Options for HOST1 used by this demo"
    exit 1
fi
if [ "$HOSTNAME2X" = "" ]
then
    $echo "Export HOSTNAME2X to set the DISPLAY Options for HOST2 used by this demo"
    exit 1
fi
#HOSTNAMEX="`hostname`:0.0"
HOSTNAME1X=$HOSTNAME1X
HOSTNAME2X=$HOSTNAME2X

TERMPATHH1="/usr/bin/xterm  -display $HOSTNAME1X"
TERMPATHH2="/usr/bin/xterm  -display $HOSTNAME2X"

$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

# 0) Install exe on HOSNAME2
BUILD_TARGET=`$CDMW_HOME/bin/config.guess`
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $CDMW_TUTORIAL_HOME/ftclock /dev/null` 

if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
   $echo Installing ftclock on $HOSTNAME2
	rsh $HOSTNAME2 mkdir -p $CDMW_TUTORIAL_HOME
	$echo "  => Copying $CDMW_TUTORIAL_HOME/ftclock"
   rcp -r $CDMW_TUTORIAL_HOME/ftclock $HOSTNAME2:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME2
fi



# 1) Start daemon 
$echo Starting the Platform Management Daemon...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"
$echo "xterm -sb -sl 7000 -e $DAEMON_COMMAND &"
xterm -sb -sl 7000 -e $DAEMON_COMMAND &

DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$CDMW_TUTORIAL_HOME/ftclock/data/CdmwPlatformMngtDaemon_conf.xml"

$echo rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH:/usr/X11R6/bin/; export PATH; $TERMPATHH2 -sb -sl 5000 -e $DAEMON_COMMAND2)" &
rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH:/usr/X11R6/bin/; export PATH; $TERMPATHH2 -sb -sl 5000 -e $DAEMON_COMMAND2)" &

trap '$DAEMON_COMMAND stop; exit' 2
#rsh $HOSTNAME2 trap '$DAEMON_COMMAND2 stop; exit' 2

sleep 2

# 1b) Start FT Manager
$echo Starting the FT Manager...
FT_CMD="$CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=../data/CdmwFaultToleranceManager_clock_conf.xml" 
$echo "$TERMPATHH1  -e $FT_CMD &"
$TERMPATHH1  -e $FT_CMD &
FT_MANAGER_PID=$!
sleep 3


$echo "Hit a key when ready to Start SuperVison.................."
read FOO
# 2) Start Platform Management Supervision
$echo "********************************************+Starting the Platform Management Supervision..."

SPV_COMMAND="$CDMW_HOME/bin/cdmw_platform_supervision --FaultManagerRegistration=corbaloc::localhost:4555/fault_manager --RequestDurationTime=20000000 --creation-timeout=20000  --CdmwLocalisationService=21871" 
$TERMPATHH1  -sb -sl 5000 -e $SPV_COMMAND &
$echo $TERMPATHH1  -sb -sl 5000 -e $SPV_COMMAND 
$echo "******************************************************************"

#SUPERVISION_PID=$!
$echo Platform Management Supervision...STARTED
sleep $INIT_TIMEOUT



# 3) Define system
$echo "*****************Defining the System "
$echo "$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE"
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

$echo "****************************************************+Defining the System "
# 4) Start_system
$echo "*****************Start the System "
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-start


$echo "=============================================================="
$echo "Hit a key when ready to stop the Primary FT Clock and test the Fault Tolerance"
read FOO
$echo "=============================================================="

$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-stop FTApplication FTClock1


if [ $? -eq "0" ];
then
    $echo "Waiting a while for the execution of the scenario...\c"
    sleep $EXEC_TIMEOUT
    $echo done.
fi

# 5) Get a snapshot of the system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-snapshot

# 6) stop_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-stop
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

