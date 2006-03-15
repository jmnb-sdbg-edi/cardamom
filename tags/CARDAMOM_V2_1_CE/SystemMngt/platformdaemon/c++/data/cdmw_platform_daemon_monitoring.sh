#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================


stop_daemon()
{
    if [ -d $2 ]; then
        echo "Kill daemon and all processes it launched"
        $1 $2
        echo "  ... done !"
    fi
    exit 1
}


# call stop_daemon at exit
trap 'stop_daemon  $1 $2' 0
    
# ignore HUP signal
trap '' 1
    
# Problem with SunOS: 
# following signal must be explicitly traped. (trap 0 is not enough !)
if [ "`uname`" = "SunOS" ]; then
    trap 'stop_daemon  $1 $2' 2 15
fi

sleep 1
while true
do
    if [ -d /proc/$PPID ]; then
	sleep 1
    else
	break
    fi
done

exit 1


