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
EOF_FILE=end_of_test.log
rm -f $EOF_FILE

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

LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$CDMW_HOME/bin:$PATH
export PATH

# XML scenario file
SCENARIO_FILE=../data/Scenario2H.xml

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

INIT_TIMEOUT=10
# interval betweek checks
EXEC_TIMEOUT=10
# maximum number of check before forced timeout (42*10=420 s = 7 min)
EXEC_STEP=42
CLEANUP_TIMEOUT=10


$echo "========================================================="
$echo "=                  CDMW PERFORMANCE TEST                ="
$echo "========================================================="
##$echo "Hit a key when ready."
##read FOO
##echo "==========================================================="

# 1.a) Start daemon on the remote host
$echo Starting the Platform Management Daemon on the remote host...
HOSTNAME2=cardamom09
TERM_PATH=/usr/bin/xterm
DAEMON_COMMAND2="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$PERFTESTS_HOME/CDMW/CDMW_Marshalling/c++/data/CdmwPlatformMngtDaemon_conf.xml"
#rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH; CDMW_HOME=$CDMW_HOME; export CDMW_HOME; PERFTESTS_HOME=$PERFTESTS_HOME; export PERFTESTS_HOME; $TERM_PATH -display $DISPLAY -sb -sl 5000 -e $DAEMON_COMMAND2)" &
rsh $HOSTNAME2 "(LD_LIBRARY_PATH=$LD_LIBRARY_PATH; export LD_LIBRARY_PATH; PATH=$PATH; export PATH;
CDMW_HOME=$CDMW_HOME; export CDMW_HOME; PERFTESTS_HOME=$PERFTESTS_HOME; export PERFTESTS_HOME;
$DAEMON_COMMAND2 &)" &

#rsh $HOSTNAME2 $DAEMON_COMMAND2 &

sleep $INIT_TIMEOUT

# 1.b) Start daemon on the local host
$echo Starting the Platform Management Daemon on the local host...
DAEMON_COMMAND="$CDMW_HOME/bin/cdmw_platform_daemon.sh --CdmwXMLFile=$DAEMON_CONF_FILE"

#$TERM_PATH -display $DISPLAY -sb -sl 5000 -e $DAEMON_COMMAND >> daemon.log &&
#$TERM -ls -sb -sl 500 -e $DAEMON_COMMAND &
#read FOO
$DAEMON_COMMAND >> daemon.log &
DAEMON_PID=$!
#trap '$DAEMON_COMMAND stop; exit' 2
sleep $INIT_TIMEOUT

# 2) Start Platform Management Supervision
$echo Starting the Platform Management Supervision...
$CDMW_HOME/bin/cdmw_platform_supervision --creation-timeout=20000 &
SUPERVISION_PID=$!
sleep $INIT_TIMEOUT

# 3) Define system
$echo Defining the System '$SCENARIO_FILE'
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define $SCENARIO_FILE

datecmd="date +%Y-%m-%d_%H:%M:%S"
# 4) Start_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-start
istep=1
if [ $? -eq "0" ];
then
    $echo "Waiting a while for the execution of the scenario...\c"
    while [ ! -f "$EOF_FILE" ] && [ $istep -le $EXEC_STEP ];
      do
      $echo `$datecmd` ": Running... " $istep  
      sleep $EXEC_TIMEOUT;
      istep=$(($istep+1))
    done
    $echo done.
fi

# 5) Get a snapshot of the system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-snapshot

# 6) stop_system
$CDMW_HOME/bin/cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-stop
sleep $CLEANUP_TIMEOUT

$echo "=============================================================="
$echo "  Stop the CDMW Daemon and Supervision.                      ="
##$echo "Hit a key when ready to stop the CDMW Daemon and Supervision."
##read FOO
$echo "=============================================================="

#$DAEMON_COMMAND stop
$DAEMON_COMMAND stop
rsh $HOSTNAME2 $DAEMON_COMMAND2 stop
kill -9 $DAEMON_PID
kill -9 $SUPERVISION_PID
$echo "done."

rm -f *.ior
rm -f *.IOR

rm -f $EOF_FILE

if [ $istep -le $EXEC_STEP ]; then
    echo "Test OK"
    exit 128
else
    echo "Error running test"
    exit 129
fi
