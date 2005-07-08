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


# 1) Start HelloServer
$echo Starting the Clock Demo...
clocksvcs&
CLOCK_SERVER_PID=$!
sleep 2
./clock.sh  -ORBInitRef ClockService=corbaloc::localhost:4883/ClockService -ORBInitRef LocalClock=corbaloc::localhost:4883/LocalClock
kill -9 $CLOCK_SERVER_PID
$echo "done."

