#!/bin/sh
#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI. All rights reserved.
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

SITE_CONFIG="$DIRECTORY/../../site.mk"

# CDMW installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"

shift

exec $DIRECTORY/../$HOST_TYPE/$PROGRAM $*

