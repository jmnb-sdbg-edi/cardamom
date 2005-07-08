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

#FILE_1=$HOME/ClockService.ior
#FILE_2=$HOME/LocalClock.ior
#echo "** Starting ClockService ** (with ORBInitRef corbaloc)"
#clocksvcs -p 4883 &
clocksvcs&
CLOCKPID=$!
# wait for initialisation
sleep 10s
testclockservice -ORBInitRef ClockService=corbaloc::localhost:4883/ClockService -ORBInitRef LocalClock=corbaloc::localhost:4883/LocalClock
TESTCLOCKPID=$!
#rm -rf $FILE_1 $FILE_2
kill -9 $CLOCKPID	2>&1 > /dev/null
kill -9 $TESTCLOCKPID   2>&1 > /dev/null
