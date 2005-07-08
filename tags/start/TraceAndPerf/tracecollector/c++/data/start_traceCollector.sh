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


# Is CDMW_RUNTIME_ROOT set?
if [ "$CDMW_RUNTIME_ROOT" = "" ]; then
    echo "You must set the CDMW_RUNTIME_ROOT variable before any work !"
    echo "This variable must contain your root directory of the CDMW CSCs"
    exit 1
fi

HOST_TYPE=`$CDMW_RUNTIME_ROOT/bin/config.guess`
HOST_SUFFIX=`echo $HOST_TYPE | cut -f 3 -d"-" | tr "." "_"`

ORB_CPP="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
COMPILER_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
COMPILER_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"
BINARY_TYPE=$HOST_TYPE-$COMPILER_NAME-$COMPILER_VER

TRACE_COLLECTOR_DIR="$CDMW_RUNTIME_ROOT/tracecollector/c++"
EXE_DIR="$TRACE_COLLECTOR_DIR/prod/$ORB_CPP/$BINARY_TYPE"

cp $TRACE_COLLECTOR_DIR/src/process.description $EXE_DIR/.

cd $EXE_DIR

$CDMW_RUNTIME_ROOT/bin/start_proc.sh $*

