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

## Usage
usage() {
    msg="usage: $my_script -b branch -e csci_export_dir -t tag"
    msg="$msg [-c csci] [-r cvsroot]"
    echo "$msg"
    echo "options:"
    echo "    -b branch           name of the branch to check out from the"
    echo "                        CVS repository"
    echo "    -e csci_export_dir  directory containing the files of a CSCI"
    echo "                        exported from the CVS repository"
    echo "    -f ECR_PCR_ref      ECR/PCR form id"
    echo "    -t tag              tag name to set when committing to CVS"
    echo "    -c csci             name of the CSCI to work on (necessary only"
    echo "                        if it cannot be guessed from csci_export_dir"
    echo "    -r cvsroot          set the root to the CVS repository"
    exit 1
}

branch=
csci_export_dir=
csci=
tag=
my_cvsroot=
ecrpcr=

## Parse command line arguments.
while getopts b:e:f:c:t:r: option; do
    case $option in
        b) branch=$OPTARG ;;
        e) if [ ! -d "$OPTARG" ]; then
               echo "$my_script: error: directory $OPTARG does not exist"
               exit 1
           fi
           # Remove possible trailing slash.
           csci_export_dir=`cd $OPTARG; pwd` ;;
        f) ecrpcr=$OPTARG ;;
        c) csci=$OPTARG ;;
        t) tag=$OPTARG ;;
        r) my_cvsroot=$OPTARG ;;
       \?) usage ;;
    esac
done

## Guess the name of the CSCI.
if [ -z "$csci" ]; then
  if [ -n "$csci_export_dir" ]; then
    csci=`basename $csci_export_dir`
  fi
fi

## Check mandatory arguments.
if [ -z "$csci_export_dir" -o -z "$csci" -o -z "$ecrpcr" ]; then
    usage
fi

# Check CVSROOT.
if [ -z "$my_cvsroot" ]; then
    if [ -z "$CVSROOT" ]; then
        echo "you need to specify a cvs root"
        exit 1
    else
        my_cvsroot=$CVSROOT
    fi
fi

## Create a work directory and checkout the branch.
my_dir=`pwd`
my_workdir=$my_dir/work
rm -rf $my_workdir; mkdir $my_workdir; cd $my_workdir
cvs -d $my_cvsroot checkout -r $branch $csci

## Identify new directories
echo "---------------- Identifying new directories"
# list 1 : list of all directories in the Branch except CVS ones
# list 2 : list of all directories in the exported area
find $csci -type d | grep -v CVS | sed -e "s%^$csci%%" | sed -e "s:^/::" | sort > list1
find $csci_export_dir -type d | sed -e "s%^$csci_export_dir%%" | sed -e "s:^/::" | sort > list2

# New dir are identified on lines begining by a single +
new_dir=`diff -u list1 list2 | grep '^+[^+]' | sed -e 's:+::'`
if [ "X$new_dir" = "X" ]
then
    echo "--------------------> No new directory."
else
    echo "--------------------> `echo $new_dir | wc -w` new dir(s)."
fi


## Identify the new files
echo "---------------- Identifying new files"
# list 1 : list of all files in the Trunc except CVS ones
# list 2 : list of all files in the exported area
find $csci -type f | grep -v CVS | sed -e "s:^$csci/::" | sort > list1
find $csci_export_dir -type f | sed -e "s:^$csci_export_dir/::" | sort > list2

# new files are listed on lines begining by a single +
new_files=`diff -u list1 list2 | grep '^+[^+]' | sed -e 's:+::'`
if [ "X$new_files" = "X" ]
then
    echo "--------------------> No new file."
else
    echo "--------------------> `echo $new_files | wc -w` new file(s)."
fi


## Identify the removed files
echo "---------------- Identifying removed files"
# deleted files are listed on lines begining by a single -
del_files=`diff -u list1 list2 | grep '^-[^-]' | sed -e 's:-::'`
if [ "X$del_files" = "X" ]
then
    echo "--------------------> No deleted file."
else
    echo "--------------------> `echo $del_files | wc -w` deleted file(s)."
fi


## Import delivery.
echo "---------------- Updating $csci, with modified and new files coming from $csci_export_dir"
\cp -Rf $csci_export_dir .

cd $csci
## Add the new directories to CVS
echo "---------------- Add New directories into CVS environment"
for d in $new_dir; do
    cvs -d $my_cvsroot add $d
done


## Add the new files to CVS.
echo "---------------- Add New files into CVS environment"
for f in $new_files; do
    cvs add $f
done


## Remove the deleted files from CVS
for f in $del_files; do
    \rm -f $f
    cvs remove $f
done

## Commit the changes into the branch and tag it.
echo "---------------- Committing $csci into the branch"
cvs commit -m "$ecrpcr: Update delivery branch with $tag delivery"

echo "---------------- Tagging branch with tag $tag"
cvs tag $tag .
