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

INIT_TIMEOUT=3
EXEC_TIMEOUT=10


$echo "========================================================="
$echo "=                  Cdmw.Tutorial.                      ="
$echo "========================================================="
$echo "Hit a key when ready."
read FOO
echo "==========================================================="

# 1) Start LBGroupManager
$echo Starting the LBGroupManager...
#$CDMW_HOME/bin/cdmw_lb_group_manager --CdmwLBXMLFile=$CDMW_HOME/share/CdmwLBGroupManager_conf.xml &
$TERM -geometry 79x20+0+0 -title groupManager -ls -sb -sl 500 -e $CDMW_HOME/bin/cdmw_lb_group_manager  --CdmwLBXMLFile=$CDMW_HOME/share/CdmwLBGroupManager_conf.xml &
LB_GROUP_MANAGER_PID=$!
sleep $INIT_TIMEOUT

# 2) Start HelloServer1
$echo Starting the Hello Server1...
$TERM -geometry 79x20+0+0 -title helloServer1 -ls -sb -sl 500 -e ./helloServer.sh  --FilePath=hello1 --HostName=helloServer1 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr &
HELLO_SERVER1_PID=$!
sleep $INIT_TIMEOUT

# 3) Start HelloServer2
$echo Starting the Hello Server2...
$TERM -geometry 79x20+500+0 -title helloServer2 -ls -sb -sl 500 -e ./helloServer.sh  --FilePath=hello2 --HostName=helloServer2 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr &
HELLO_SERVER2_PID=$!
sleep $INIT_TIMEOUT

# 4) Start HelloServer3
$echo Starting the Hello Server3...
$TERM -geometry 79x20+0+292 -title helloServer3 -ls -sb -sl 500 -e ./helloServer.sh  --FilePath=hello3 --HostName=helloServer3 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr &
HELLO_SERVER3_PID=$!
sleep $INIT_TIMEOUT

# 5) Start HelloServer4
$echo Starting the Hello Server4...
$TERM -geometry 79x20+500+292 -title helloServer4 -ls -sb -sl 500 -e ./helloServer.sh  --FilePath=hello4 --HostName=helloServer4 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr &
HELLO_SERVER4_PID=$!
sleep $INIT_TIMEOUT

# 6) Start GroupCreator
$echo Starting the GroupCreator...
./groupCreator.sh -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr &
GROUP_CREATOR_PID=$!
sleep $INIT_TIMEOUT

# 7) Start HelloClient
$echo Starting the Hello Client...
$TERM -geometry 158x28+0+584 -title helloClient -ls -sb -sl 500 -e ./helloClient.sh &  
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
kill -9 $HELLO_SERVER1_PID $LB_GROUP_MANAGER_PID $HELLO_SERVER2_PID $HELLO_SERVER3_PID $HELLO_SERVER4_PID 
$echo "done."

rm -f *.ior
rm -f *.IOR
