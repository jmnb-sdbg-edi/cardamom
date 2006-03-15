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

# define macro echo
if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi

# handle input parameters
if [[ $# -ne 5 && $# -ne 6 ]]
then
    $echo "usage: $0 FederatedControlledExecutor <policy> <display> <port1> <port2> <port3>"
    $echo "usage: $0 FederatedControlledClock <display> <port1> <port2> <port3>"
    $echo "where: <policy>  can be CANCEL_ALL|ENFORCE_INTERVAL|ENFORCE_DEADLINE"
    $echo "       <display> specifies the display environment"
    $echo "       <port1>   specifies the port used for the 1-st timeout handler, time reader and time admin"
    $echo "       <port2>   specifies the port used for the 2-nd timeout handler, time reader and time admin"
    $echo "       <port3>   specifies the port used for the 3-rd timeout handler, time reader and time admin"
    exit 1
fi
if [[ ("$1" != "FederatedControlledClock") && ("$1" != "FederatedControlledExecutor") ]]
then
    $echo "$0: clock name can only be FederatedControlledClock or FederatedControlledExecutor"
    exit 1
fi
if [[ ($# -eq 6) && ("$2" != "CANCEL_ALL") && ("$2" != "ENFORCE_INTERVAL") && ("$2" != "ENFORCE_DEADLINE") ]]
then
    $echo "$0: policy can only be CANCEL_ALL, ENFORCE_INTERVAL or ENFORCE_DEADLINE"
    exit 1
fi
SITE_CONFIG=../site.mk
if [[ ! -f "$SITE_CONFIG" ]]
then
    $echo "$0: $SITE_CONFIG file is missing. Run '../configure.sh' and restart $0"
    exit 1
fi

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"
LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH

# Target dependent build directory name
BUILD_TARGET=`$CDMW_HOME/bin/config.guess`

# Macro defining the used terminal
if [[ ! -x `which $TERM` ]]
then
    TERM=xterm
fi
TERM_PATH=`which $TERM`

# Macro defining the input arguments
CLOCK_NAME=$1
if [[ $# -eq 6 ]]
then
    POLICY=$2
    DISPLAY=$3
    PORT1=$4
    PORT2=$5
    PORT3=$6
fi
if [[ $# -eq 5 ]]
then
    DISPLAY=$2
    PORT1=$3
    PORT2=$4
    PORT3=$5
fi

# Check for already executing timeout handlers, time readers and time admins
PIDS_TH=`/sbin/pidof TimeoutHandler`
PIDS_TR=`/sbin/pidof TimeReader`
PIDS_TA=`/sbin/pidof TimeAdmin`
if [[ ("$PIDS_TH" != "") || ("$PIDS_TR" != "") || ("$PIDS_TA" != "") ]]
then
    $echo "$0: timeout handlers or time readers or time admins already running"
    $echo "$0: demo aborted"
    exit 1
fi

# Run the demo
$echo "========================================================="
$echo "=                  CDMW DEMO                           ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

START_TIME=`../$BUILD_TARGET/GetCurrentTime`
START_TIME=$(($START_TIME+200000000))

# 1) Start timeout handlers 
$echo Starting the timeout handlers...

exec $TERM -ls -sb -sl 1000 -geometry 50X10+000+000 -display $DISPLAY -title "TimeoutHandler:$PORT1" -e ../$BUILD_TARGET/TimeoutHandler $START_TIME --CdmwMessageResend=1 --CdmwFederatedCtrlClock --CdmwLocalisationService=$PORT1 --CdmwClockName=$CLOCK_NAME --TimerPolicy=$POLICY &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+330+000 -display $DISPLAY -title "TimeoutHandler:$PORT2" -e ../$BUILD_TARGET/TimeoutHandler $START_TIME --CdmwMessageResend=1 --CdmwFederatedCtrlClock --CdmwLocalisationService=$PORT2 --CdmwClockName=$CLOCK_NAME --TimerPolicy=$POLICY &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+660+000 -display $DISPLAY -title "TimeoutHandler:$PORT3" -e ../$BUILD_TARGET/TimeoutHandler $START_TIME --CdmwMessageResend=1 --CdmwFederatedCtrlClock --CdmwLocalisationService=$PORT3 --CdmwClockName=$CLOCK_NAME --TimerPolicy=$POLICY &

sleep 5

# 2) Start the time readers
$echo Starting the time readers...

exec $TERM -ls -sb -sl 1000 -geometry 50X10+000+160 -display $DISPLAY -title "TimeReader:$PORT1" -e ../$BUILD_TARGET/TimeReader -ORBInitRef ClockService=corbaloc::localhost:$PORT1/ClockService --CdmwClockName=$CLOCK_NAME &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+330+160 -display $DISPLAY -title "TimeReader:$PORT2" -e ../$BUILD_TARGET/TimeReader -ORBInitRef ClockService=corbaloc::localhost:$PORT2/ClockService --CdmwClockName=$CLOCK_NAME &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+660+160 -display $DISPLAY -title "TimeReader:$PORT3" -e ../$BUILD_TARGET/TimeReader -ORBInitRef ClockService=corbaloc::localhost:$PORT3/ClockService --CdmwClockName=$CLOCK_NAME &

sleep 1

# 3) Start the time admins
$echo Starting the time admins...

exec $TERM -ls -sb -sl 1000 -geometry 50X10+000+320 -display $DISPLAY -title "TimeAdmin:$PORT1" -e ../$BUILD_TARGET/TimeAdmin -ORBInitRef ClockService=corbaloc::localhost:$PORT1/ClockService --CdmwClockName=$CLOCK_NAME &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+330+320 -display $DISPLAY -title "TimeAdmin:$PORT2" -e ../$BUILD_TARGET/TimeAdmin -ORBInitRef ClockService=corbaloc::localhost:$PORT2/ClockService --CdmwClockName=$CLOCK_NAME &
exec $TERM -ls -sb -sl 1000 -geometry 50X10+660+320 -display $DISPLAY -title "TimeAdmin:$PORT3" -e ../$BUILD_TARGET/TimeAdmin -ORBInitRef ClockService=corbaloc::localhost:$PORT3/ClockService --CdmwClockName=$CLOCK_NAME &

sleep 1

# 4) Stop all processes when ready
$echo "=============================================================="
$echo "Hit a key when ready to stop the following processes:"
$echo "1. the timeout handlers,"
$echo "2. the time readers,"
$echo "3. the time admins."
read FOO
$echo "=============================================================="

# 4.1) Stop time admins, if any
PIDS_TA=`/sbin/pidof TimeAdmin`
if [[ "$PIDS_TA" != "" ]]
then
    kill -9 $PIDS_TA
fi

# 4.2) Stop time readers, if any
PIDS_TR=`/sbin/pidof TimeReader`
if [[ "$PIDS_TR" != "" ]]
then
    kill -9 $PIDS_TR
fi

# 4.3) Stop timeout handlers, if any
PIDS_TH=`/sbin/pidof TimeoutHandler`
if [[ "$PIDS_TH" != "" ]]
then
    kill -9 $PIDS_TH
fi

$echo "done."
