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

print_error()
{
cat >&2 << __EOF__
$program: error: $*
__EOF__
}


ZIP=zip
UNZIP=unzip
OVERWRITE_ARG=

# Check zip existence
$ZIP -h  >/dev/null 2>&1

if test $? -ne 0; then
    print_error "$ZIP: no such file. Please update your PATH environment variable."
    exit 1
fi

# Check unzip existence
$UNZIP -v  >/dev/null 2>&1

if test $? -ne 0; then
    print_error "$UNZIP: no such file. Please update your PATH environment variable."
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
  echo "                   ...   done"
#  error 20
}


usage()
{
cat >&2 << __EOF__
Usage: ${program} [--overwrite] --cad=FILENAME [--cpf=FILENAME]* (--component-pakage=FILENAME)+

Options:
--help                 Show this message.
--overwrite            Overwrite existing files from component packages without prompting.
                       This should appear before any other option to be applied to all
                       component packages
--cad=FILENAME         Use FILENAME as the Component Assembly Descriptor
--cpf=FILENAME         Add FILENAME as a Component or Home property file.
                       Use this option for each property file.
--component=FILENAME   Add Component Package FILENAME to the assembly.
                       Use this option for each component package.

This tool generates an Assembly Package Archive with .aar extension.
__EOF__

}


if test "$#" -lt 2
then
    usage $*
    exit 1
fi


# clean tmp directory
rm -rf $pack_tmpdir
mkdir $pack_tmpdir
mkdir $pack_tmpdir/meta-inf


cad_file=""
component_found=""

cat >&2 << __EOF__
-- CDMW ASSEMBLY PACKAGE ARCHIVE GENERATOR --
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
    --overwrite)
        OVERWRITE_ARG="-o"
        ;;
    --cad=*)
        if test "x$cad_file" != x; then
            print_error "Only one CSD file allowed!"
            cleanup
            exit 1
        fi
        # Check .cad extension
        if test "`basename $optarg`" = "`basename $optarg .cad`"; then            
            print_error "CAD file requires .cad extension"
            cleanup
            exit 1
        fi
        cp -p $optarg $pack_tmpdir/meta-inf
        if test $? -ne 0; then
            print_error "Failed to copy $optarg"
            cleanup
            exit 1
        fi
        cad_file=$optarg
        ;;
    --component=*)
        if test -r $optarg; then
            $UNZIP -j $OVERWRITE_ARG $optarg -d $pack_tmpdir
            if test $? -ne 0; then
                print_error "Failed to unzip $optarg"
                cleanup
                exit 1
            fi
        else
            print_error "Failed to read $optarg"
            cleanup
            exit 1
        fi
        component_found="true"
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


if test "x$cad_file" = x; then
    print_error "CAD file is missing!"
    cleanup
    exit 1
fi
if test "x$component_found" = x; then
    print_error "Component packages are missing!"
    cleanup
    exit 1
fi


CWD=`pwd`
cd $pack_tmpdir
archive=`basename $cad_file .cad`.aar
echo Creating assembly package archive $archive...
$ZIP -r $CWD/$archive *
echo Assembly Package Archive written to $CWD/$archive 
cd $CWD

rm -rf $pack_tmpdir

