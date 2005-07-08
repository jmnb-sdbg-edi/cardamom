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


#----------------------------------------------------------------------
#  This shell script launches the Trace Administration Console
#----------------------------------------------------------------------

# Is CDMW_START_SIMU_CONF set? The file may initialise REPPORT env variables
if [ -f "$CDMW_START_SIMU_CONF" ]; then
    . $CDMW_START_SIMU_CONF
fi

# Is CDMW_RUNTIME_ROOT set?
if [ "$CDMW_RUNTIME_ROOT" = "" ]; then
    echo "You must set the CDMW_RUNTIME_ROOT variable before any work !"
	echo "This variable must contain your root directory of the CDMW CSCs"
	exit 1
fi


###
# TOOLS executables
HOST_TYPE=`$CDMW_RUNTIME_ROOT/bin/config.guess`
HOST_SUFFIX=`echo $HOST_TYPE | cut -f 3 -d"-" | tr "." "_"`
#HOST_TYPE=$HOST_TYPE-$CXX_NAME-$CXX_VER


ORB_CPP="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
COMPILER_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
COMPILER_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"
BINARY_TYPE=$HOST_TYPE-$COMPILER_NAME-$COMPILER_VER

# set admin_command file with host name
HOST_NAME=`hostname`

in_file="admin_command.cmd.in"

# The output is the input file stripped from its .in suffix
out_file="`echo $in_file  | sed s%\\.in$%%`"

# if output file already exists, we remove it
if test -f $out_file
then
    rm $out_file
fi

echo "Generating $out_file"

cat $in_file | sed -e "s%@HOSTNAME@%$HOST_NAME%g" >> $out_file
    
if test -x $in_file
then
    chmod a+x $out_file
fi

# start trace administration

TRACEADMIN_EXE=$CDMW_RUNTIME_ROOT/traceadmin/c++/prod/$ORB_CPP/$BINARY_TYPE/cdmw_trace_admin
###

REPOSITORY_URL=corbaloc::localhost:$REPPORT/SimRepository

$TRACEADMIN_EXE --namingRepos-url=$REPOSITORY_URL --commandFile-name=admin_command.cmd



