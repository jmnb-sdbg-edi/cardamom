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
    echo "This script kill all processes which have their pid saved into .pid files into the specified directory, and delete those files."
    echo "The directory is explored recursively."
    echo "Usage:"
    echo "   $0 <pid_files_directory>"
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

pid_dir=$1
shift


# test id pid_dir is a directory
if [ ! -d $pid_dir ]; then
    echo "Error: $pid_dir is not a directory !"
    print_usage
    exit $ret_nok
fi


# for each .pid file: kill pid and delete .pid file
for file in `find $pid_dir -name "*.pid"`; do

    pid=`cat $file 2> /dev/null`
    
    if [ "x$pid" != "x" ]; then
        kill $pid 2> /dev/null
    fi
      
    # if the process is supervised, the pid file could be removed
    # before the following rm. Thus, ignore error message.
    rm -f $file 2> /dev/null
    
done


# try to rmdir recursively (fail if directories are not empty)
for dir in `find $pid_dir -type d -depth`; do
    rmdir $dir
done


exit $ret_ok


