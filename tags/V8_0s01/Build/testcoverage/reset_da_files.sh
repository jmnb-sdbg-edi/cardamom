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
    echo "$my_script: Set all gcov execution counter to zero in *.da files."
    echo
    echo "usage: $my_script -c cdmw_home [-k]"
    echo
    echo "options:"
    echo "    -c <cdmw_home>  Cardamom sources directory."
    echo "    [-k]            Only reset non-existing .da files. Keep existing."
    exit 1
}
 


## Parse command line arguments.
while getopts c:k option; do
    case $option in
        c) cdmw_home=$OPTARG ;;
        k) keep_existing=true ;;
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

if [ ! -d $cdmw_home -o ! -d $cdmw_home/Build/testcoverage ]; then
    echo "$cdmw_home is not the Cardamom sources directory"
    usage
fi


if [ -z "$keep_existing" ]; then
    find $cdmw_home -name "*.bb" | sed "s%.bb%.da%" | xargs rm -f
else
    echo "keep existing .da files intact."
fi

_tmpfile_=`mktemp /tmp/${my_script}.XXXXXX`
find $cdmw_home -name "*.bb" | sed "s%.bb%.da%" > $_tmpfile_
cat $_tmpfile_ | xargs touch
cat $_tmpfile_ | xargs chmod a+rw
rm $_tmpfile_
