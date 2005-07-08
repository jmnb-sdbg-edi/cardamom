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
#===============================================================================


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


if test ! -x `which $TERM`
then
    TERM=xterm
fi

INIT_TIMEOUT=5
EXEC_TIMEOUT=10


$echo "========================================================="
$echo "=                  Cdmw.Tutorial.                      ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

# 1) Start naming and repository 
$echo Starting the Naming and Repository ...
$TERM -ls -sb -sl 500 -e $CDMW_HOME/bin/cdmw_naming_and_repository --CdmwProcessPort=42561 --CdmwLocalisationService=42562 --id=GlobalRepository --conf=../data/Cdmw_nar.xml &
REPOSITORY_PID=$!

sleep $INIT_TIMEOUT

# 2) Start trace collector
$echo Starting the Trace Collector...
$TERM -ls -sb -sl 500 -e $CDMW_HOME/bin/cdmw_trace_collector --collector-name=the_collector --namingRepos-url=corbaloc::localhost:42562/CdmwRepository &
TRACE_COLLECTOR_PID=$!
sleep $INIT_TIMEOUT


# 4) Start HelloServer
$echo Starting the Hello Server...
$TERM -ls -sb -sl 500 -e ./helloServer.sh --namingRepos-url=corbaloc::localhost:42562/CdmwRepository --collector-name=the_collector &
HELLO_SERVER_PID=$!

if [ $? -eq "0" ];
then
    $echo "Waiting a while for the execution of the scenario...\c"
    sleep $EXEC_TIMEOUT
    $echo done.
fi


$echo "============================="
$echo "Hit a key when ready to stop."
read FOO
$echo "============================="
kill -9 $HELLO_SERVER_PID $TRACE_COLLECTOR_PID $REPOSITORY_PID
$echo "done."

rm -f *.ior
rm -f *.IOR
