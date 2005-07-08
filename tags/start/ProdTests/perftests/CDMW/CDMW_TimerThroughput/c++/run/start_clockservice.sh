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

PATH=$PATH:/home/cardamom/CDMW-Install/bin
FILE_1=ClockService.ior
FILE_2=LocalClock.ior
echo "** Launch CARDAMOM ClockService**"
clocksvcs -o $FILE_1 -o $FILE_2 -p 4883 &
CLOCKPID=$!
echo "************ NOW I WILL SET THE PRIORITY **************************"
echo "************ NOW I WILL SET THE PRIORITY **************************"
echo "************ NOW I WILL SET THE PRIORITY **************************"
echo "************ NOW I WILL SET THE PRIORITY **************************"
echo "************ Clocksvc PID" $CLOCKPID "###################"
/usr/bin/sudo /home/cardamom/ProdTests/perftests/CDMW/RTNice/rtnice $CLOCKPID 99 
/usr/bin/sudo /home/cardamom/ProdTests/perftests/CDMW/RTNice/rtget $CLOCKPID 
# wait for initialisation
sleep 1
