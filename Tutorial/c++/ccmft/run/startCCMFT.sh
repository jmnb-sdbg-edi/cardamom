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

PWD=`pwd`
DEMO_DIR=`echo $PWD | sed s%/run%%g`
SITE_CONFIG=../../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../configure.sh' and restart $0 "
    exit 1
fi


##########################################################
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


# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME= |cut -d= -f2`"
if [ "$CDMW_HOME" = "" ]
then
    $echo "CDMW_HOME is not set into $SITE_CONFIG"
    exit 1
fi

# CDMW Tutorial folder
CDMW_TUTORIAL_HOME="`cat $SITE_CONFIG | grep CDMW_TUTORIAL_HOME= |cut -d= -f2`"
if [ "$CDMW_TUTORIAL_HOME" = "" ]
then
    $echo "CDMW_TUTORIAL_HOME is not set into $SITE_CONFIG"
    exit 1
fi

rm -fr workDir
rsh $HOSTNAME2 rm -fr $CDMW_TUTORIAL_HOME/ccmft/run/workDir
rsh $HOSTNAME3 rm -fr $CDMW_TUTORIAL_HOME/ccmft/run/workDir

# FTP DIR
FTP_DIR="`cat $SITE_CONFIG | grep FTP_DIR= |cut -d= -f2`"
if [ "$FTP_DIR" = "" ]
then
    $echo "FTP_DIR is not set into $SITE_CONFIG"
    exit 1
fi

rm -fr $FTP_DIR/cdmw_ccm_ft
rsh $HOSTNAME2 rm -fr $FTP_DIR/cdmw_ccm_ft
rsh $HOSTNAME3 rm -fr $FTP_DIR/cdmw_ccm_ft

LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH


#######################################

# XML scenario file
SCENARIO_FILE=../data/ScenarioCcmFt.xml

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
START_TIMEOUT=25


$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="

# 0a) Check Cardamom & demo installation on host2
BUILD_TARGET=`$CDMW_HOME/bin/config.guess`
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME2 ls $CDMW_TUTORIAL_HOME/ccmft/$BUILD_TARGET 2>/dev/null` 

if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
   $echo Installing the ccm_ft example on $HOSTNAME2
	rsh $HOSTNAME2 mkdir -p $CDMW_HOME
	$echo "  => Copying $CDMW_HOME/bin"
   rcp -r $CDMW_HOME/bin $HOSTNAME2:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/lib"
   rcp -r $CDMW_HOME/lib $HOSTNAME2:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/share"
   rcp -r $CDMW_HOME/share $HOSTNAME2:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/idl"
   rcp -r $CDMW_HOME/idl $HOSTNAME2:$CDMW_HOME
	rsh $HOSTNAME2 mkdir -p $CDMW_TUTORIAL_HOME/c++
	$echo "  => Copying $CDMW_TUTORIAL_HOME/ccmft"
   rcp -r $CDMW_TUTORIAL_HOME/ccmft $HOSTNAME2:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME2
fi


# 0b) Check Cardamom & demo installation on host3
BUILD_TARGET=`$CDMW_HOME/bin/config.guess`
DEMO_ALREADY_INSTALLED=`rsh $HOSTNAME3 ls $CDMW_TUTORIAL_HOME/ccmft/$BUILD_TARGET 2>/dev/null` 

if [ "$DEMO_ALREADY_INSTALLED" = "" ]
then
   $echo Installing the ccm_ft example on $HOSTNAME3
	rsh $HOSTNAME3 mkdir -p $CDMW_HOME
	$echo "  => Copying $CDMW_HOME/bin"
   rcp -r $CDMW_HOME/bin $HOSTNAME3:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/lib"
   rcp -r $CDMW_HOME/lib $HOSTNAME3:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/share"
   rcp -r $CDMW_HOME/share $HOSTNAME3:$CDMW_HOME
	$echo "  => Copying $CDMW_HOME/idl"
   rcp -r $CDMW_HOME/idl $HOSTNAME3:$CDMW_HOME
	rsh $HOSTNAME3 mkdir -p $CDMW_TUTORIAL_HOME
	$echo "  => Copying $CDMW_TUTORIAL_HOME/ccmft"
   rcp -r $CDMW_TUTORIAL_HOME/ccmft $HOSTNAME3:$CDMW_TUTORIAL_HOME
else
   $echo Demo already installed on $HOSTNAME3
fi




# 1) Start daemon 
$echo Starting the Platform Management Daemon...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"
TERM_COMMAND1="$TERM -sb -sl 5000 -geometry 90x30+0+0 -title ${HOSTNAME}_platform_daemon"
$TERM_COMMAND1 -e $DAEMON_COMMAND &


DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DEMO_DIR/data/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND2="$TERM -sb -sl 5000 -geometry 90x30-0+0 -title ${HOSTNAME2}_platform_daemon -display $DISPLAY"
rsh $HOSTNAME2 "(export LANG=$LANG; export TAO_DISABLE_CODESET_ENCODING=$TAO_DISABLE_CODESET_ENCODING; LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; TAO_ROOT=$TAO_ROOT; export TAO_ROOT; PATH=$PATH; export PATH; $TERM_COMMAND2 -e $DAEMON_COMMAND2)" &

DAEMON_COMMAND3="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DEMO_DIR/data/CdmwPlatformMngtDaemon_conf.xml"
TERM_COMMAND3="$TERM -sb -sl 5000 -geometry 90x30-0-0 -title ${HOSTNAME3}_platform_daemon -display $DISPLAY"
rsh $HOSTNAME3 "(cd $CDMW_TUTORIAL_HOME/ccmft/run; export LANG=$LANG; export TAO_DISABLE_CODESET_ENCODING=$TAO_DISABLE_CODESET_ENCODING; LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; TAO_ROOT=$TAO_ROOT; export TAO_ROOT; PATH=$PATH; export PATH; $TERM_COMMAND3 -e $DAEMON_COMMAND3)" &

trap '$DAEMON_COMMAND stop; exit' 2
#rsh $HOSTNAME2 trap '$DAEMON_COMMAND2 stop; exit' 2

sleep $INIT_TIMEOUT

# 1b) Start FT Manager
$echo Starting the FT Manager...
#FT_MANAGER_COMMAND="$CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=$DEMO_DIR/data/CdmwFaultToleranceManager_ccmft_conf.xml --groupConf=$DEMO_DIR/data/CdmwFTSystemMngtGroupCreator_conf.xml ; cat"
#TERM_FT="$TERM -sb -sl 5000 -geometry 90x30-0-0 -title ${HOSTNAME3}_ft_manager -display $DISPLAY"
# if we want to launch FTManager on HOST3, in this case change corbaloc refs in Scenario
#rsh $HOSTNAME3 "(export LANG=$LANG; export TAO_DISABLE_CODESET_ENCODING=$TAO_DISABLE_CODESET_ENCODING; LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; TAO_ROOT=$TAO_ROOT; export TAO_ROOT; PATH=$PATH; export PATH; $TERM_FT -e $FT_MANAGER_COMMAND)" &
$TERM -ls -sb -sl 2000 -e $CDMW_HOME/bin/cdmw_ft_manager --CdmwXMLFile=$DEMO_DIR/data/CdmwFaultToleranceManager_ccmft_conf.xml --groupConf=$DEMO_DIR/data/CdmwFTSystemMngtGroupCreator_conf.xml&
FT_MANAGER_PID=$!
sleep $FT_MANAGER_TIMEOUT

# 2) Start Platform Management Supervision
#$echo Starting the Platform Management Supervision...
#$CDMW_HOME/bin/cdmw_platform_supervision --CdmwLocalisationService=21888 --FaultManagerRegistration=corbaloc::$HOST3:4565/fault_manager --RequestDurationTime=20000000 --creation-timeout=20000 &
#SUPERVISION_PID=$!
#sleep $INIT_TIMEOUT

# 2) Start Platform Management Supervision
$CDMW_HOME/bin/cdmw_platform_supervision_starter --CdmwXMLFile=../data/CdmwPlatformMngtSystemStart.xml --validate
sleep $INIT_TIMEOUT

# 3) Define system
$echo Defining the System '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

# 4) Start_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-start
if [ $? -eq "0" ];
then
    #$echo "Waiting a while for the execution of the scenario...\c"
    #sleep $EXEC_TIMEOUT
    $echo "Hit a key when the scenario is ended...\c"
    sleep $START_TIMEOUT
    $echo done.
fi

# 5) Get a snapshot of the system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-snapshot

# 6) Execute deployment
$echo
$echo ============================
$echo Execute deployment commands
$echo ============================
$echo
$TERM -ls -sb -sl 1000 -e ./startDeploymentTool.sh&


$echo "=============================================================="
$echo "Hit a key when ready to stop the System.                      "
read FOO
$echo "=============================================================="

# 7) stop_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21880/CdmwPlatformMngtSupervision --sys-stop
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

