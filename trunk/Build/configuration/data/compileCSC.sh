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


my_script=`basename $0`

## Usage.
usage() {
    msg="usage: $my_script [-d | --debug] [-v | --verbose]"
    msg="$msg [--sloppy-check] [--color] [-h | --help]"
    msg="$msg [--use-ccache]"
    echo "$msg"
    exit 1
}

ant_args="-q -emacs"
ant_logger="-logger com.thalesgroup.tools.ant.listener.CompilationLogger"
sloppy_check=
ignore_ccache="-Dignore.ccache=true"

## Parse command line arguments.
args=`getopt -o dvh -l debug,verbose,sloppy-check,use-ccache,color,help  - "$@"`

eval set -- "$args"
for i; do
    case "$i" in
        -d | --debug)
            shift
            ant_args=
            ;;
        -v | --verbose)
            shift
            ant_args="-v"
            ;;
        --color)
            shift
            ant_logger="-logger org.apache.tools.ant.listener.AnsiColorLogger"
            ;;
        --sloppy-check)
            shift
            sloppy_check="-Dsloppy.check=true"
            ;;
        --use-ccache)
            shift
            ignore_ccache="-Dignore.ccache=false"
            ;;
        -h | --help)
            shift
            usage
            ;;
    esac
done

## Look for an ANT build file.
ant_build_file=`find . -name "Build*.xml" -maxdepth 1`

## Execute the default target.
if [ -n "$ant_build_file" ]; then
    ANT_OPTS="-Xmx500M"
    CLASSPATH=@CDMWTASKS_JAR@:$CLASSPATH
    export ANT_OPTS CLASSPATH
    ant -f $ant_build_file $ant_args $sloppy_check $ant_logger $ignore_ccache \
        -Dforce.recompilation=true
else
    echo "Could not find any ANT build file."
    exit 1
fi
