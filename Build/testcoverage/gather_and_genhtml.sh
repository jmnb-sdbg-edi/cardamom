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

 
my_script=`basename $0`
current_dir=`pwd` 

filter_file="default_gcov_filter.txt"


## Usage
usage() {
    echo "$my_script: Gather and merge gcov information from different hosts, "
    echo "and use lcov to generate a html report."
    echo
    echo "usage: $my_script -o output [-f filter_file] [-h host1,...,hostN]"
    echo
    echo "options:"
    echo "    -o <output-dir>      Name of the directory used for lcov generation."
    echo "                         This directory is created if it doesn't exist."
    echo "    -f <filter_file>     Name of the file containing wildcards for extracting"
    echo "                         files or directories from the generated results."
    echo "                         Default file is $filter_file"
    echo "    -h <host1,...,hostN> List of remote hosts on which gcov infos" 
    echo "                         should be gathered."
    exit 1
}


check_lcov() {
    lcov_dir=`which lcov` 2> /dev/null
    if [ -z $lcov_dir ]; then
        echo "lcov tool should be installed and present in PATH."
        exit 1
    fi
}
 
 
check_host() {
    host_to_check=$1
    cdmw_home=$2
    h=`rsh $host_to_check uname -n`
    if [ -z "$h" ]; then
        echo "Cannot reach host $host_to_check"
        exit 1
    fi
    
    _tmp_=`rsh $host_to_check ls -al $cdmw_home 2> /dev/null`
    if [ -z "$_tmp_" ]; then
        echo "Cannot find $cdmw_home on $host_to_check"
        exit 1
    fi
} 

 

### Check current directory
if [ ! -x $my_script ]; then
    echo "This script should be started from the Build/testcoverage directory."
    exit 1
fi

cdmw_home=${current_dir%Build/testcoverage}

### Check lcov
check_lcov


## Parse command line arguments.
while getopts o:f:h: option; do
    case $option in
        o) output_dir=$OPTARG ;;
        f) filter_file=$OPTARG ;;
        h) remote_hosts=$OPTARG ;;
       \?) usage ;;
    esac
done

if [ "$OPTIND" -le "$#" ]
then
   shift $(($OPTIND-1))
   echo "ERROR on input parameter $1"
   usage
   exit 1
fi

if [ -z "$output_dir" ]; then
    echo "Missing -o option."
    usage
fi 
if [ ! -d $output_dir ]; then
    mkdir -p $output_dir
    if [ $? != 0 ]; then
        exit 1
    fi
fi

remote_hosts=`echo $remote_hosts | sed "s%,% %"`

for i in $remote_hosts; do
    check_host $i $cdmw_home
done


####
echo out dir = $output_dir
echo filter_file = $filter_file
echo remote_hosts = $remote_hosts
echo cdmw_home = $cdmw_home
####

### gather on localhost
lcov_local_file=$output_dir/infos_localhost.lcov
echo " gather gcov infos on localhost"
./gather_local_infos.sh -c $cdmw_home -o $lcov_local_file -f $filter_file


### gather on remote host
remote_gathering() {
    host=$1
    cdmw_home=$2
    filter_file=$3
    output_dir=$4
    
    remote_filter=`rsh $host mktemp /tmp/${my_script}.XXXXXX`
    remote_lcov=`rsh $host mktemp /tmp/${my_script}.XXXXXX`
    echo " copy filter on $host"
    echo "     remote_filter=$remote_filter"
    echo "     remote_lcov=$remote_lcov"
    rcp $filter_file $host:$remote_filter
    echo " gather infos on $host"
    rsh $host "( export PATH=$PATH; cd $current_dir; ./gather_local_infos.sh -c $cdmw_home -o $remote_lcov -f $remote_filter )"
    echo " copy infos from $host"
    echo "     into $output_dir/infos_${host}.lcov"
    rcp $host:$remote_lcov $output_dir/infos_${host}.lcov
    
    rsh $host rm $remote_filter
    rsh $host rm $remote_lcov
}


### gather on all remote hosts
for i in $remote_hosts; do
    remote_gathering $i $cdmw_home $filter_file $output_dir
done

lcov_final=$output_dir/all_infos.lcov
lcov_add_option="-a $lcov_local_file"
for i in $remote_hosts; do
    lcov_add_option="$lcov_add_option -a ${output_dir}/infos_${i}.lcov"
done

### create final lcov file
echo " aggregate all gcov infos"
echo "  > lcov -o $lcov_final $lcov_add_option"
lcov -o $lcov_final $lcov_add_option


### generate html report
echo " generate html report in $output_dir"
genhtml -k -s -o $output_dir $lcov_final


