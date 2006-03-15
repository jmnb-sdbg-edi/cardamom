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
    msg="usage: $my_script -p product_release_tag -e product_export_dir -m commit_message"
    msg="$msg [-c csci1,...,csciN] [-r cvsroot] [-n]"
    echo "$msg"
    echo "options:"
    echo "    -p <product_release_tag>   : Tag for all CSCIs included into the"
    echo "                                 Product Release."
    echo "                                 For instance : CARDAMOM_V0_1, CARDAMOM_V1_0_CE, ..."
    echo "    -e <product_export_dir>    : Directory containing all the CSCIs to be"
    echo "                                 included into the repository"
    echo "    -m <commit_message>        : Message added for commit into the trunc"
    echo "    -c <csci1,...,csciN>       : In addition of the modified CSCIs processed by this script, just "
    echo "                                 tag the csci1,...csciN CSCIs with the same tag."
    echo "    -r <cvsroot>               : Set the root to the CVS repository."
    echo "    -n                         : Process without performing any modification into the cvs repository."
    exit 1
}

## Execute_protected
execute_protected() {
    if [ "$cvsprocessing" = "yes" ]
    then
        $@
    else
        echo -e "Command = $@"
    fi
}

branch=
product_release_tag=
product_export_dir=
trunc_tag=
commit_message=
additionalcscis=
my_cvsroot=
cvsprocessing="yes"
tmp_dir="/tmp/work"

## Parse command line arguments.
while getopts c:e:m:np:r: option; do
    case $option in
        c) additionalcscis=$OPTARG ;;
        e) if [ ! -d "$OPTARG" ]; then
               echo "$my_script: error: directory $OPTARG does not exist"
               exit 1
           fi
           # Remove possible trailing slash.
           product_export_dir=`cd $OPTARG; pwd` ;;
        m) commit_message=$OPTARG ;;
        n) cvsprocessing="no" ;;
        p) product_release_tag=$OPTARG ;;
        r) my_cvsroot=$OPTARG ;;
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

## check mandatory arguments.
if [ -z "$product_release_tag" -o -z "$product_export_dir" -o -z "$commit_message" ]; then
    usage
fi

## Check CVSROOT.
if [ -z "$my_cvsroot" ]; then
    if [ -z "$CVSROOT" ]; then
        echo "you need to specify a cvs root"
        exit 1
    else
        my_cvsroot=$CVSROOT
    fi
fi

## Determine the list of CSCIs to process
cd $product_export_dir
csci_list=`find . -type d -mindepth 1 -maxdepth 1 -print | sed -e "s/\.\///g"`

## Process each CSCI
for csci in `echo $csci_list`; do
    echo -e "\n\n**************** Processing CSCI $csci"
    cd $product_export_dir

    ## Determine tag name and csci revision number
    csci_rev="`grep csci $csci/data/CSCIProperties.xml | \
               grep name | grep version |\
               sed -e 's/^.*version=\"//g' |\
               sed -e 's/\".*$//g' |\
               sed -e 's/\./_/g'`" 
    trunc_tag="V`echo $csci_rev`"

    ## Checkout the Trunc
    echo "---------------- Checkout Trunc for $csci in $tmp_dir/$csci"
    rm -rf $tmp_dir
    mkdir $tmp_dir
    cd $tmp_dir
    cvs -d $my_cvsroot checkout $csci
    if [ "$?" = "0" ]; then
       # csci exists into the repository

       ## Identify new directories
       echo "---------------- Identifying new directories"
       # list 1 : list of all directories in the Trunc except CVS ones
       # list 2 : list of all directories in the exported area
       find $tmp_dir/$csci -type d | grep -v CVS | sed -e "s%^$tmp_dir/$csci%%" | sed -e "s:^/::" | sort > $tmp_dir/list1
       find $product_export_dir/$csci -type d | sed -e "s%^$product_export_dir/$csci%%" | sed -e "s:^/::" | sort > $tmp_dir/list2

       # New dir are identified on lines begining by a single + 
       new_dir=`diff -u $tmp_dir/list1 $tmp_dir/list2 | grep '^+[^+]' | sed -e 's:+::'`
       if [ "X$new_dir" = "X" ]
       then
           echo "--------------------> No new directory."
       else
           echo "--------------------> `echo $new_dir | wc -w` new dir(s)."
       fi 

       ## Identify the new and removed files
       echo "---------------- Identifying new files"
       # list 1 : list of all files in the Trunc except CVS ones
       # list 2 : list of all files in the exported area
       find $tmp_dir/$csci -type f | grep -v CVS  | sed -e "s%^$tmp_dir/$csci/%%" | sort > $tmp_dir/list1
       find $product_export_dir/$csci -type f | sed -e "s%^$product_export_dir/$csci/%%" | sort > $tmp_dir/list2

       # new files are listed on lines begining by a single + 
       new_files=`diff -u $tmp_dir/list1 $tmp_dir/list2 | grep '^+[^+]' | sed -e 's:+::'`
       if [ "X$new_files" = "X" ]
       then
           echo "--------------------> No new file."
       else
           echo "--------------------> `echo $new_files | wc -w` new file(s)."
       fi 

       ## Identify the removed files
       echo "---------------- Identifying removed files"
       # deleted files are listed on lines begining by a single - 
       del_files=`diff -u $tmp_dir/list1 $tmp_dir/list2 | grep '^-[^-]' | sed -e 's:-::'`
       if [ "X$del_files" = "X" ]
       then
           echo "--------------------> No deleted file."
       else
           echo "--------------------> `echo $del_files | wc -w` deleted file(s)."
       fi 

       ## update the local instance of the trunc
       echo "---------------- Updating $tmp_dir/$csci, with modified and new files"
       \cp -Rf $product_export_dir/$csci $tmp_dir

       cd $tmp_dir/$csci
       ## Add the new directories to CVS
       echo "---------------- Add New directories into CVS environment"
       for d in $new_dir; do
           execute_protected cvs -d $my_cvsroot add $d
       done

       ## Add the new files to CVS.
       echo "---------------- Add New files into CVS environment"
       for f in $new_files; do
           execute_protected cvs -d $my_cvsroot add $f
       done

       ## Remove the deleted files from CVS
       echo "---------------- Remove removed files from CVS environment"
       for f in $del_files; do
           execute_protected \rm -f $f
           execute_protected cvs -d $my_cvsroot remove $f
       done

       cd $tmp_dir
       ## Commit the trunc
       echo "---------------- Committing $csci into the Trunc"
       if [ "$cvsprocessing" = "yes" ]
       then
           cvs -d $my_cvsroot commit -m "$commit_message" $csci
       else
           echo -e "Command = cvs -d $my_cvsroot commit -m \"$commit_message\" $csci"
       fi

       ## Tag the Trunc
       echo "---------------- Tagging $csci with tag $trunc_tag"
       execute_protected cvs -d $my_cvsroot rtag $trunc_tag $csci 
       echo "---------------- Tagging $csci with tag $product_release_tag"
       execute_protected cvs -d $my_cvsroot rtag $product_release_tag $csci 

    else
       # the csci does not exist into the repository
       echo "----------------> $csci doesn't exist. Import it and tag it $trunc_tag"
       if [ "$cvsprocessing" = "yes" ]
       then
           cd $product_export_dir/$csci
           cvs -d $my_cvsroot import -m "$commit_message" $csci InitialVersion $trunc_tag
       else
           echo "cd $product_export_dir/$csci"
           echo -e "Command = cvs -d $my_cvsroot import -m \"$commit_message\" $csci InitialVersion $trunc_tag"
       fi

       ## Tag the Trunc
       echo "---------------- Tagging $csci with tag $product_release_tag"
       execute_protected cvs -d $my_cvsroot rtag $product_release_tag $csci 
    fi

done

#Tag the additional CSCIs if any
if [ "X$additionalcscis" != "X" ]
then
    echo -e "\n\n**************** Tagging additional CSCIs"
    csci_list=`echo $additionalcscis | sed -e "s/,/ /g"`
    for csci in $csci_list; do
        echo "---------------- Tagging $csci with tag $product_release_tag"
        execute_protected cvs -d $my_cvsroot rtag $product_release_tag $csci 
    done
fi
