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

SITE_CONFIG=../../site.mk

if test ! -f "$SITE_CONFIG"
then
    $echo "$SITE_CONFIG file is missing. Run '../../configure.sh' and restart $0 "
    exit 1
fi

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"


if test ! -x `which $TERM`
then
    TERM=xterm
fi

INIT_TIMEOUT=5
EXEC_TIMEOUT=10


$echo "========================================================="
$echo "=                  CDMW TUTORIAL                       ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="


# 1) Start HelloServer
$echo Starting the Hello Server...
$TERM -ls -sb -sl 500 -e ./helloServer.sh &
HELLO_SERVER_PID=$!
sleep $INIT_TIMEOUT

# 2) Start HelloClient
$echo Starting the Hello Client...
$TERM -ls -sb -sl 500 -e ./helloClient.sh &
HELLO_CLIENT_PID=$!

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
kill -9 $HELLO_SERVER_PID $HELLO_CLIENT_PID
$echo "done."

rm -f *.ior
rm -f *.IOR
