#!/bin/sh
#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
#* =========================================================================== *

if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi


PWD=`pwd`
DEMO_DIR=`echo $PWD | sed s%/run%%g`
SITE_CONFIG=../../site.mk

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

# Check HOSTNAME1 is set
HOSTNAME1="`echo $HOSTNAME1`"
if [ "$HOSTNAME1" = "" ]
then
    $echo "HOSTNAME1 is not set into $SITE_CONFIG"
    exit 1
fi

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

LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH

# XML scenario file
SCENARIO_FILE=../data/ScenarioDemoLB.xml


if test ! -f "$SCENARIO_FILE"
then
    $echo "$SCENARIO_FILE file is missing. Run 'make data' in parent directory restart $0 "
    exit 1
fi

# XML Platform Management Daemon configuration file
DAEMON_CONF_FILE=$CDMW_HOME/share/CdmwPlatformMngtDaemon_conf.xml

if test ! -f "$DAEMON_CONF_FILE"
then
    $echo "$DAEMON_CONF_FILE file is missing. Run 'make data' in parent directory restart $0 "
    exit 1
fi


if test ! -x `which $TERM`
then
    TERM=xterm
fi
# CDMW Tutorial folder
CDMW_TUTORIAL_HOME="`cat $SITE_CONFIG | grep CDMW_TUTORIAL_HOME= |cut -d= -f2`"
if [ "$CDMW_TUTORIAL_HOME" = "" ]
then
    $echo "CDMW_TUTORIAL_HOME is not set into $SITE_CONFIG"
    exit 1
fi

TERM_PATH=`which $TERM`

INIT_TIMEOUT=20
FT_MANAGER_TIMEOUT=20
EXEC_TIMEOUT=160
CLEANUP_TIMEOUT=5

HOSTNAME=`hostname`

$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="

# 0a) Check Cardamom & demo installation on host1

CDMW_ALREADY_INSTALLED=`rsh $HOSTNAME1 ls $CDMW_HOME/bin/cdmw_platform_daemon 2>/dev/null`
if [ "$CDMW_ALREADY_INSTALLED" = "" ]
then
     $echo Installing CARDAMOM on $HOSTNAME1
	rsh $HOSTNAME1 mkdir -p $CDMW_HOME
	$echo "  => Copying $CDMW_HOME/bin"
   rcp -r $CDMW_HOME/bin $HOSTNAME1:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/lib"
   rcp -r $CDMW_HOME/lib $HOSTNAME1:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/share"
   rcp -r $CDMW_HOME/share $HOSTNAME1:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/idl"
   rcp -r $CDMW_HOME/idl $HOSTNAME1:$CDMW_HOME
else 
   $echo CARDAMOM already installed on $HOSTNAME1
fi


# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME1 ls $DEMO_DIR/$BUILD_TARGET  2>/dev/null`
if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
rsh $HOSTNAME1 mkdir -p $CDMW_TUTORIAL_HOME
   $echo "  => Copying demo lb"
   rcp -r $CDMW_TUTORIAL_HOME/demo_lb $HOSTNAME1:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME1

fi


# 0a) Check Cardamom & demo installation on host2

CDMW_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $CDMW_HOME/bin/cdmw_platform_daemon 2>/dev/null`
if [ "$CDMW_ALREADY_INSTALLED" = "" ]
then
    $echo Installing CARDAMOM on $HOSTNAME2
    rsh $HOSTNAME2 mkdir -p $CDMW_HOME
    $echo "  => Copying $CDMW_HOME/bin"
    rcp -r $CDMW_HOME/bin $HOSTNAME2:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/lib"
    rcp -r $CDMW_HOME/lib $HOSTNAME2:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/share"
    rcp -r $CDMW_HOME/share $HOSTNAME2:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/idl"
    rcp -r $CDMW_HOME/idl $HOSTNAME2:$CDMW_HOME
else 
    $echo CARDAMOM already installed on $HOSTNAME2
fi



# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $DEMO_DIR/$BUILD_TARGET 2>/dev/null`
if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
rsh $HOSTNAME2 mkdir -p $CDMW_TUTORIAL_HOME
   $echo "  => Copying demo lb"
   rcp -r $CDMW_TUTORIAL_HOME/demo_lb $HOSTNAME2:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME2

fi

# 0b) Check Cardamom & demo installation on host3

CDMW_ALREADY_INSTALLED=`rsh $HOSTNAME3 ls $CDMW_HOME/bin/cdmw_platform_daemon 2>/dev/null`
if [ "$CDMW_ALREADY_INSTALLED" = "" ]
then
$echo Installing CARDAMOM on $HOSTNAME3
    rsh $HOSTNAME3 mkdir -p $CDMW_HOME
    $echo "  => Copying $CDMW_HOME/bin"
    rcp -r $CDMW_HOME/bin $HOSTNAME3:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/lib"
    rcp -r $CDMW_HOME/lib $HOSTNAME3:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/share"
    rcp -r $CDMW_HOME/share $HOSTNAME3:$CDMW_HOME
    $echo "  => Copying $CDMW_HOME/idl"
    rcp -r $CDMW_HOME/idl $HOSTNAME3:$CDMW_HOME
else 
    $echo CARDAMOM already installed on $HOSTNAME3
fi

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME3 ls $DEMO_DIR/$BUILD_TARGET 2>/dev/null`
if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
rsh $HOSTNAME3 mkdir -p $CDMW_TUTORIAL_HOME
   $echo "  => Copying demo lb"
   rcp -r $CDMW_TUTORIAL_HOME/demo_lb $HOSTNAME3:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME3

fi


# 1) Start daemon 
$echo Starting the Platform Management Daemon on $HOSTNAME1...
DAEMON_COMMAND1="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$CDMW_HOME/share/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND1="$TERM -sb -sl 5000 -display $DISPLAY -geometry 95x21+0+0 -bg rgb:87/ce/fa -title ${HOSTNAME1}_platform_daemon "
rsh $HOSTNAME1 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_COMMAND1 -e $DAEMON_COMMAND1)" &

$echo Starting the Platform Management Daemon on $HOSTNAME2...
DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$CDMW_HOME/share/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND2="$TERM -sb -sl 5000 -display $DISPLAY -geometry 95x21-0+0 -bg rgb:87/ce/fa -title ${HOSTNAME2}_platform_daemon "
rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_COMMAND2 -e $DAEMON_COMMAND2)" &

$echo Starting the Platform Management Daemon on $HOSTNAME3...
DAEMON_COMMAND3="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$CDMW_HOME/share/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND3="$TERM -sb -sl 5000 -display $DISPLAY -geometry 95x21-0+312 -bg rgb:87/ce/fa -title ${HOSTNAME3}_platform_daemon "
rsh $HOSTNAME3 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_COMMAND3 -e $DAEMON_COMMAND3)" &

sleep $INIT_TIMEOUT

# 1b) Start FT Manager
$echo Starting the FT Manager...
$TERM -sb -sl 5000 -geometry 95x21+0+312 -bg rgb:f0/e6/8c -title ${HOSTNAME}_ft_manager -e $CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=$CDMW_HOME/share/CdmwFaultToleranceManager_conf.xml --groupConf=$DEMO_DIR/data/CdmwFTSystemMngtGroupCreator_conf.xml &
FT_MANAGER_PID=$!
sleep $FT_MANAGER_TIMEOUT


# 2) Start Platform Management Supervision
$echo Starting the Platform Management Supervision...
$TERM -sb -sl 5000 -geometry 95x21+0-0 -bg rgb:f4/a4/60 -title ${HOSTNAME}_platform_supervision -e $CDMW_HOME/bin/cdmw_platform_supervision --FaultManagerRegistration=corbaloc::localhost:4555/fault_manager --CdmwXMLFile=$CDMW_HOME/share/CdmwSystemMngtDatastoreConfig.xml --creation-timeout=20000 --RequestDurationTime=20000000 --CdmwLocalisationService=21871 &
SUPERVISION_PID=$!
sleep $INIT_TIMEOUT



# 2b) Start LBGroupManagerSupervisionObserver
$echo Starting the LB Group Manager Supervision Observer...
TERM_LB_COMMAND="$TERM -sb -sl 5000 -bg rgb:d8/bf/d8 -display $DISPLAY -geometry 95x21-0-0 -title ${HOSTNAME2}_LBGroupManagerSupervisionObserver "
LB_GROUP_MANAGER_OBSERVER_COMMAND="$CDMW_HOME/bin/start_lb_group_manager_observer --system-corbaloc=corbaloc::$HOSTNAME:21871/CdmwPlatformMngtSupervision --CdmwLBXMLFile=$CDMW_HOME/share/CdmwLBGroupManager_conf.xml" 
rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; $TERM_LB_COMMAND -e $LB_GROUP_MANAGER_OBSERVER_COMMAND) " &
sleep $INIT_TIMEOUT



# 3) Define system
$echo Defining the System '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

# 4) Start_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-start
if [ $? -eq "0" ];
then
    #$echo "Waiting a while for the execution of the scenario...\c"
    #sleep $EXEC_TIMEOUT
    $echo "Hit a key when the scenario is ended...\c"
    read FOO
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

rsh $HOSTNAME1 $DAEMON_COMMAND1 stop
rsh $HOSTNAME2 $DAEMON_COMMAND2 stop
rsh $HOSTNAME3 $DAEMON_COMMAND3 stop
kill -9 $SUPERVISION_PID
kill -9 $FT_MANAGER_PID
$echo "done."

rm -f *.ior
rm -f *.IOR


