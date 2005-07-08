#!/bin/sh
#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#* 
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *


# Port numbers
REPPORT=5021
COLPORT1=5051
COLPORT2=5052

# Is CDMW_START_SIMU_CONF set? The file may initialise REPPORT env variables
if [ -f "$CDMW_START_SIMU_CONF" ]; then
    . $CDMW_START_SIMU_CONF
fi

#----------------------------------------------------------------------
#  This shell script launches the Trace Collector  
#----------------------------------------------------------------------
usage()
{
    echo "Usage: $0  [options]"
    echo "option:"
    echo "    --help"
    echo "             Print this help"
    echo "    --repository-port port "
    echo "             Simulated Repository lauched at the specified port"
    echo "             Default is $REPPORT"
    echo "    --collector-service-port port "
    echo "             Trace Collector service lauched at the specified port"
    echo "             Default is $COLPORT1"
    echo "    --collector-cdmw-port port "
    echo "             Trace Collector lauched at the specified cdmw port"
    echo "             Default is $COLPORT2"
    echo "Description:"
    echo "      This program starts the Trace Collector."
}



# Is CDMW_RUNTIME_ROOT set?
if [ "$CDMW_RUNTIME_ROOT" = "" ]; then
    echo "You must set the CDMW_RUNTIME_ROOT variable before any work !"
	echo "This variable must contain your root directory of the CDMW CSCs"
	exit 1
fi


while [ $# -gt 0 ]
do
    case $1 in
        --help)
            usage $0
            exit 0
            ;;    
        --repository-port)
            shift        
            if [ $# -gt 0 ]
            then
                REPPORT="$1"
                shift
            fi
            ;;  
        --collector-service-port)
            shift        
            if [ $# -gt 0 ]
            then
                COLPORT1="$1"
                shift
            fi
            ;;  
        --collector-cdmw-port)
            shift        
            if [ $# -gt 0 ]
            then
                COLPORT2="$1"
                shift
            fi
            ;;  
        *)
            shift
            ;;
  esac
done



###
# TOOLS executables
HOST_TYPE=`$CDMW_RUNTIME_ROOT/bin/config.guess`
HOST_SUFFIX=`echo $HOST_TYPE | cut -f 3 -d"-" | tr "." "_"`
#HOST_TYPE=$HOST_TYPE-$CXX_NAME-$CXX_VER


ORB_CPP="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
COMPILER_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
COMPILER_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"
BINARY_TYPE=$HOST_TYPE-$COMPILER_NAME-$COMPILER_VER

EXE_DIR="prod/$ORB_CPP/$BINARY_TYPE"

LD_LIBRARY_PATH=$CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$BINARY_TYPE:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

# start trace collector exe

TRACECOLLECTOR_EXE=$CDMW_RUNTIME_ROOT/tracecollector/c++/$EXE_DIR/cdmw_trace_collector
###

REPOSITORY_URL=corbaloc::localhost:$REPPORT/SimRepository

$TRACECOLLECTOR_EXE --interactive --collector-name=collector_4 --namingRepos-url=$REPOSITORY_URL



