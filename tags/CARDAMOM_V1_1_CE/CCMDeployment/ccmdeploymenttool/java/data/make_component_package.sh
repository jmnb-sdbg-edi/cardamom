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


program=`basename ${0}`
tmp_archive=$$.zip

print_error()
{
cat >&2 << __EOF__
$program: error: $*
__EOF__
}


ZIP=zip

# Check zip existence
$ZIP -h  >/dev/null 2>&1

if test $? -ne 0; then
    print_error "$ZIP: no such file. Please update your PATH environment variable."
    exit 1
fi


pack_tmpdir=/tmp/$$

trap 'cleanup' 1 2 3 4 5 6 7 8 15

#
# if the user stops the script (kill), here is the cleanup procedure
#
cleanup ()
{
  echo "Performing cleanup ..."
  rm -rf $pack_tmpdir
  rm -f $tmp_archive
  echo "                   ...   done"
#  error 20
}


usage()
{
cat >&2 << __EOF__
Usage: ${program} --csd=FILENAME --ccd=FILENAME (--idl=FILENAME)+ (--lib=FILENAME)+ [--cpf=FILENAME] [--cpf=FILENAME]

Options:
--help                 Show this message.
--csd=FILENAME         Use FILENAME as the Component Software Descriptor.
--ccd=FILENAME         Add FILENAME as a CORBA Component Descriptor.
--idl=FILENAME         Add FILENAME IDL file to the archivbe
                       Use this option for each IDL file.
--lib=FILENAME         Add FILENAME shared library to the archive.
                       Use this option for each shared library (such as
                       valuetypefactory dependencies)
--cpf=FILENAME         Add Component property file FILENAME.
--cpf=FILENAME         Add Home property file FILENAME.

This tool generates a Component Package Archive with a .zip extension.
__EOF__

}


if test "$#" -lt 4
then
    usage $*
    exit 1
fi


# clean tmp directory
rm -rf $pack_tmpdir
mkdir $pack_tmpdir
mkdir $pack_tmpdir/meta-inf


csd_file=""
ccd_found=""
idl_found=""
lib_found=""

cat >&2 << __EOF__
-- CDMW COMPONENT PACKAGE ARCHIVE GENERATOR --
__EOF__

for option
do
    case "$option" in
    --*=*) optarg=`echo "$option" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
    *) optarg= ;;
    esac
    
    case "$option" in

    --help)
        usage $*
        exit 0
        ;;
    --csd=*)
        if test "x$csd_file" != x; then
            print_error "Only one CSD file allowed!"
            cleanup
            exit 1
        fi
        # Check .csd extension
        if test "`basename $optarg`" = "`basename $optarg .csd`"; then            
            print_error "CSD file requires .csd extension"
            cleanup
            exit 1
        fi
        cp -p $optarg $pack_tmpdir/meta-inf
        if test $? -ne 0; then
            print_error "Failed to copy $optarg"
            cleanup
            exit 1
        fi
        csd_file=$optarg
        ;;
    --ccd=*)
        if test "x$ccd_found" != x; then
            print_error "Only one CCD file allowed!"
            cleanup
            exit 1
        fi
        cp -p $optarg $pack_tmpdir
        if test $? -ne 0; then
            print_error "Failed to copy $optarg"
            cleanup
            exit 1
        fi
        ccd_found="true"
        ;;
    --idl=*)
        cp -p $optarg $pack_tmpdir
        if test $? -ne 0; then
            print_error "Failed to copy $optarg"
            cleanup
            exit 1
        fi
        idl_found="true"
        ;;
    --lib=*)
        # make links for libraries
        if test -r $optarg; then
            $ZIP -rj $tmp_archive $optarg
        else
            print_error "Failed to read $optarg"
            cleanup
            exit 1
        fi
        lib_found="true"
        ;;
    --cpf=*)
        cp -p $optarg $pack_tmpdir
        if test $? -ne 0; then
            print_error "Failed to copy $optarg"
            cleanup
            exit 1
        fi
        ;;
    *)
        ;;
  esac
done


if test "x$csd_file" = x; then
    print_error "CSD file is missing!"
    cleanup
    exit 1
fi
if test "x$ccd_found" = x; then
    print_error "CCD file is missing!"
    cleanup
    exit 1
fi
if test "x$idl_found" = x; then
    print_error "IDL file are missing!"
    cleanup
    exit 1
fi
if test "x$lib_found" = x; then
    print_error "Shared libraries are missing!"
    cleanup
    exit 1
fi


CWD=`pwd`
cd $pack_tmpdir
archive=`basename $csd_file .csd`.zip
echo Creating component archive $archive...
$ZIP -r $CWD/$tmp_archive *
cd $CWD
mv $tmp_archive $archive
echo Component Package Archive written to $CWD/$archive 

rm -rf $pack_tmpdir

