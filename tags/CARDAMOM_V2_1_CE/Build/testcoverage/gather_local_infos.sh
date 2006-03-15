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
    echo "$my_script: Gather all gcov informations from Cardamom compilation"
    echo "directory, and use lcov to generate a filtered trace file."
    echo
    echo "usage: $my_script -c cdmw_home -o output_file -f filter_file"
    echo
    echo "options:"
    echo "    -c <cdmw_home>     Cardamom sources directory."
    echo "    -o <output-file>   File to be generated."
    echo "    -f <filter_file>   File containing wildcards for extracting"
    echo "                       files or directories from the generated results."
    exit 1
}
 

check_lcov() {
    lcov_dir=`which lcov` 2> /dev/null
    if [ -z $lcov_dir ]; then
        echo "lcov tool should be installed and present in PATH."
        exit 1
    fi
}
 
 

### Check current directory
if [ ! -x $my_script ]; then
    echo "This script should be started from the Build/testcoverage directory."
    exit 1
fi


### Check lcov
check_lcov


## Parse command line arguments.
while getopts c:o:f: option; do
    case $option in
        c) cdmw_home=$OPTARG ;;
        o) output_file=$OPTARG ;;
        f) filter_file=$OPTARG ;;
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


### check options
if [ -z "$cdmw_home" ]; then
    echo "Missing -c option."
    usage
fi
if [ -z "$output_file" ]; then
    echo "Missing -o option."
    usage
fi
if [ -z "$filter_file" ]; then
    echo "Missing -f option."
    usage
fi

if [ ! -d $cdmw_home -o ! -d $cdmw_home/Build/testcoverage ]; then
    echo "$cdmw_home is not the Cardamom sources directory"
    usage
fi

touch $output_file
if [ ! -f $output_file ]; then
    usage
fi
rm $output_file

if [ ! -f $filter_file ]; then
    echo "Filter file $filter_file not found"
    usage
fi


### read filter file 
### (removing comment lines and replacing * with \*) 
filter=
while read i; do
    if [ "`echo $i`x" == "`echo $i | grep -v \"#\"`x" ]; then
        j="`echo \"$i\" | sed -e \"s%*%\\\\\*%g\" `"
        filter="$filter $j"
    fi
done < $filter_file 


### reserve tmp file
tmp_file=`mktemp /tmp/${my_script}.XXXXXX`
rm $tmp_file

### gather all gcov infos from localhost
echo "  > lcov -c -d $cdmw_home -o $tmp_file"
lcov -c -d $cdmw_home -o $tmp_file 2>&1 \
    | grep -v "Use of uninitialized value in join or string" \
    | grep -v "cannot find an entry for"

    
### filter gcov infos
echo "  > lcov -r $tmp_file -o $output_file $filter"
eval lcov -r $tmp_file -o $output_file $filter

rm $tmp_file

