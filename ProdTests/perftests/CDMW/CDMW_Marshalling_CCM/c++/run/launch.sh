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

SELF=`basename $0`

if [ "$#" = "0" ]; then

cat >&2 <<EOF
Usage : $SELF program [arg1 ...]
 
EOF
    exit 1
fi

DIRECTORY="`dirname $0`"
PROGRAM="$1"

SITE_CONFIG="$DIRECTORY/../site.mk"

# CDMW installation folder
###CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"

CDMW_PERF_HOME="`cat $SITE_CONFIG | grep CDMW_PERF_HOME |cut -d= -f2`"
LD_LIBRARY_PATH=$CDMW_HOME/lib/c++:$CDMW_PERF_HOME/../../cTools/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH


shift

exec $DIRECTORY/../$HOST_TYPE/$PROGRAM $*

