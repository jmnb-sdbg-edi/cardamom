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
if [[ $# -ne 3 && $# -ne 4 ]]
then
    $echo "usage: $0 FederatedControlledExecutor <policy> <display> <port>"
    $echo "usage: $0 FederatedControlledClock <display> <port>"
    $echo "where: <policy>  can be CANCEL_ALL|ENFORCE_INTERVAL|ENFORCE_DEADLINE"
    $echo "       <display> specifies the display environment"
    $echo "       <port>    specifies the port used for the new timeout handler and time reader"
    exit 1
fi
if [[ ("$1" != "FederatedControlledClock") && ("$1" != "FederatedControlledExecutor") ]]
then
    $echo "$0: clock name can only be FederatedControlledClock or FederatedControlledExecutor"
    exit 1
fi
if [[ ($# -eq 4) && ("$2" != "CANCEL_ALL") && ("$2" != "ENFORCE_INTERVAL") && ("$2" != "ENFORCE_DEADLINE") ]]
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
if [[ $# -eq 4 ]]
then
    POLICY=$2
    DISPLAY=$3
    PORT=$4
fi
if [[ $# -eq 3 ]]
then
    DISPLAY=$2
    PORT=$3
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

# 1) Start timeout handler 
$echo Starting the timeout handler...

exec $TERM -ls -sb -sl 1000 -geometry 50X10+000+000 -display $DISPLAY -title "TimeoutHandler:$PORT" -e ../$BUILD_TARGET/TimeoutHandler $START_TIME --CdmwMessageResend=1 --CdmwFederatedCtrlClock --CdmwLocalisationService=$PORT --CdmwClockName=$CLOCK_NAME --TimerPolicy=$POLICY &

sleep 5

# 2) Start the time reader
$echo Starting the time reader...

exec $TERM -ls -sb -sl 1000 -geometry 50X10+000+160 -display $DISPLAY -title "TimeReader:$PORT" -e ../$BUILD_TARGET/TimeReader -ORBInitRef ClockService=corbaloc::localhost:$PORT/ClockService --CdmwClockName=$CLOCK_NAME &

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
