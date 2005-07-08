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


ret_ok=0;
ret_nok=-1;

print_usage()
{
    echo "This script kill a process which has its pid saved into the specified file, and delete this file."
    echo "Usage:"
    echo "   $0 <pid_file>"
    echo " "
}



if [ "$*" = "" ]; then
    print_usage
    exit $ret_nok;
fi

if [ "$1" = "--help" ]; then
    print_usage
    exit $ret_ok;
fi

pid_file=$1
shift

sleep 5

if [ -f $pid_file ]; then
    pid=`cat $pid_file 2> /dev/null`
    
    if [ "x$pid" != "x" ]; then
        kill -9 $pid 2> /dev/null
    fi
    
    rm -f $pid_file
fi

exit $ret_ok


