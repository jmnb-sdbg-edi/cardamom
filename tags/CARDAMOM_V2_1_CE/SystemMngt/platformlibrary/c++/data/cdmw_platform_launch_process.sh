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


ret_ok=0;
ret_nok=-1;

print_usage()
{
    echo "This script launch a command, and write its PID into a specified file"
    echo "Usage:"
    echo "   $0 <pid_file> <command>"
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

if [  -f $pid_file ]; then
    echo "ici $pid_file"
    echo >&2 "Error: pid_file '$pid_file' already exists"
    exit $ret_nok;    
fi

if [ "$*" = "" ]; then
    echo >&2 "Error: no command specified"
    print_usage
    exit $ret_nok;
fi 

touch_result=`touch $pid_file 2>&1`

if [ "$touch_result" != "" ]; then
    echo  >&2 "Error: pid_file '$pid_file' cannot be created"
    exit $ret_nok;
fi
   
the_pid=$$
    
(echo $the_pid > $pid_file)
exec $*


