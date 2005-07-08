#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES 
# and SELEX-SI. 
# 
# It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
# All rights reserved.
# 
# CARDAMOM is free software; you can redistribute it and/or modify it under 
# the terms of the GNU Library General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your 
# option) any later version. 
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
# License for more details. 
# 
# You should have received a copy of the GNU Library General 
# Public License along with CARDAMOM; see the file COPYING. If not, write to 
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================


if test "`echo -e xxx`" = "xxx"
then
    echo="echo -e"
else
    echo=echo
fi


# Is CDMW_RUNTIME_ROOT set?
if [ "$CDMW_RUNTIME_ROOT" = "" ]; then
    $echo "You must set the CDMW_RUNTIME_ROOT variable before any work !"
	$echo "This variable must contain your root directory of the CDMW CSCs"
	exit 1
fi

if test ! -x `which $TERM`
then
    TERM=xterm
fi



$echo ""
$echo "Note: runtest.sh must have been done for namingandrepository"
$echo " before starting this test"
$echo ""

# configuration file file
site_file=$CDMW_RUNTIME_ROOT/site.mk

# We determine the target
HOST_TYPE="`$CDMW_RUNTIME_ROOT/bin/config.guess`"

# We determine which ORB, compiler are to be used for the c++ test
ORB_CPP="`cat $site_file | grep ORB_CPP |cut -d= -f2`"
CXX_NAME="`cat $site_file | grep CXX_NAME |cut -d= -f2`"
CXX_VER="`cat $site_file | grep CXX_VER |cut -d= -f2`"
ORBACUS_BIN_PATH="`cat $site_file | grep ORBACUS_BIN_PATH |cut -d= -f2`"
BIN_CXX=$HOST_TYPE-$CXX_NAME-$CXX_VER
EXE_DIR="prod/$ORB_CPP/$BIN_CXX"

NAMINGANDREPOSITORY_PATH=$CDMW_RUNTIME_ROOT/namingandrepository/c++


NAMINGANDREPOSITORY_EXE_PATH=$NAMINGANDREPOSITORY_PATH/$EXE_DIR/
NAMINGANDREPOSITORY_TEST2_PATH=$NAMINGANDREPOSITORY_PATH/test/test2/$EXE_DIR/
NAMINGANDREPOSITORY_TEST3_PATH=$NAMINGANDREPOSITORY_PATH/test/test3/$EXE_DIR/


#
# start the naming and repository
cd $NAMINGANDREPOSITORY_EXE_PATH
exec $TERM -ls -sb -sl 500 -e ./cdmw_naming_and_repository --CdmwLocalisationService=4997 --id=GlobalRepository --conf=CdmwDefaultNamingAndRepository.xml --threading-test &
NAMING_SERVER_PID=$!

sleep 5

# 
# start the 1st bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT1_PID=$!

sleep 2

# 
# start the 1st cleaner client
cd $NAMINGANDREPOSITORY_TEST3_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextReaderCleaner &
CCLIENT1_PID=$!

sleep 2

# 
# start the 2nd bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT2_PID=$!

sleep 2

# 
# start the 3rd bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT3_PID=$!

sleep 2


# 
# start the 4th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT4_PID=$!

sleep 2


# 
# start the 5th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT5_PID=$!

sleep 2


# 
# start the 6th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT6_PID=$!

sleep 2

# 
# start the 7th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT7_PID=$!

sleep 2


# 
# start the 8th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e NamingContextBinder &
BCLIENT8_PID=$!

sleep 2


# 
# start the 9th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT9_PID=$!

sleep 2


# 
# start the 10th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT10_PID=$!

sleep 2


# 
# start the 11th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT11_PID=$!

sleep 2

# 
# start the 12th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT12_PID=$!

sleep 2


# 
# start the 13th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT13_PID=$!

sleep 2


# 
# start the 14th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT14_PID=$!

sleep 2


# 
# start the 15th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT15_PID=$!

sleep 2


# 
# start the 16th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT16_PID=$!

sleep 2

# 
# start the 17th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT17_PID=$!

sleep 2


# 
# start the 18th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT18_PID=$!

sleep 2


# 
# start the 19th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT19_PID=$!

sleep 2



# 
# start the 20th bind client
cd $NAMINGANDREPOSITORY_TEST2_PATH
exec $TERM -ls -sb -sl 500 -e ./NamingContextBinder &
BCLIENT20_PID=$!

sleep 2






$echo "============================="
$echo "Hit a key when ready to stop."
read FOO
$echo "============================="
kill -9 $NAME_SERVER_PID
kill -9 $BCLIENT1_PID $BCLIENT2_PID $BCLIENT3_PID $BCLIENT4_PID $BCLIENT5_PID
kill -9 $BCLIENT6_PID $BCLIENT7_PID $BCLIENT8_PID $BCLIENT9_PID $BCLIENT10_PID
kill -9 $BCLIENT11_PID $BCLIENT12_PID $BCLIENT13_PID $BCLIENT14_PID $BCLIENT15_PID
kill -9 $BCLIENT16_PID $BCLIENT17_PID $BCLIENT18_PID $BCLIENT19_PID $BCLIENT20_PID
kill -9 $CCLIENT1_PID
$echo "done."


