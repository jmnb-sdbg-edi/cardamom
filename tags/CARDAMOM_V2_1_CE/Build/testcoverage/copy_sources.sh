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


## Usage
usage() {
    echo "$my_script: Copy all Cardamom sources (excluding generated, prod and delivery directories), "
    echo "on the specified hosts, into the same absolute directory than the sources"
    echo
    echo "usage: $my_script -c cdmw_home -h host1,...,hostN"
    echo
    echo "options:"
    echo "    -c <cdmw_home>        Cardamom sources directory."
    echo "    -h <host1,...,hostN>  File to be generated."
    exit 1
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
    if [ ! -z "$_tmp_" ]; then
        echo "WARNING: $cdmw_home already exist on $host_to_check. Remove it before."
        exit 1
    fi
} 


copy_and_extract() {
    tgz_file=$1
    host=$2
    cdmw_home=$3
    
    rsh $host mkdir -p $cdmw_home
    _tmp_=`rsh $host ls -al $cdmw_home 2> /dev/null`
    if [ -z "$_tmp_" ]; then
        echo "WARNING: Cannot create directory $cdmw_home on $host."
        return
    fi

    echo "    copy archive on $host..."
    rcp $tgz_file $host:$cdmw_home/archive.tgz
    
    echo "    extract archive on $host..."
    rsh $host tar xzf $cdmw_home/archive.tgz --absolute-names
    rsh $host rm $cdmw_home/archive.tgz

    echo "    create empty .da files on $host..."
    rsh $host $cdmw_home/Build/testcoverage/reset_da_files.sh -c $cdmw_home
}


## Parse command line arguments.
while getopts c:h: option; do
    case $option in
        c) cdmw_home=$OPTARG ;;
        h) remote_hosts=$OPTARG ;;
       \?) usage ;;
    esac
done

if [ "$OPTIND" -le "$#" ]
then
   shift $(($OPTIND-1))
   echo "ERROR on input parameter $1"
   echo ""
   usage
   exit 1
fi

if [ -z "$cdmw_home" ]; then
    echo "Missing -c option."
    echo ""
    usage
fi
if [ -z "$remote_hosts" ]; then
    echo "Missing -h option."
    echo ""
    usage
fi

if [ ! -d $cdmw_home -o ! -d $cdmw_home/Build/testcoverage ]; then
    echo "$cdmw_home is not the Cardamom sources directory"
    echo ""
    usage
fi


remote_hosts=`echo $remote_hosts | sed "s%,% %"`

for i in $remote_hosts; do
    check_host $i $cdmw_home
done

echo "  find files to be copied..."
tmp_files_to_copy=`mktemp /tmp/${my_script}_files_to_copy.XXXXXX`
find $cdmw_home -name "*.bb" -o -name "*.bbg" -o -name "*.?pp" -o -name "*.h" -o -name "*.i" -o -name "testcoverage"> $tmp_files_to_copy


echo "  create zipped archive for transfer..."
tmp_tgz=`mktemp /tmp/${my_script}_tgz.XXXXXX`
tar czf $tmp_tgz --files-from $tmp_files_to_copy --absolute-names

for i in $remote_hosts; do
    copy_and_extract $tmp_tgz $i $cdmw_home
done


rm $tmp_tgz
rm $tmp_files_to_copy

echo "  done."
