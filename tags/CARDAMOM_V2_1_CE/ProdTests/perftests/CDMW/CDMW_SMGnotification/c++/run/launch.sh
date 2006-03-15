#!/bin/sh
#* =========================================================================== *
#* THALES (R) - CARDAMOM
#*
#* Copyright (c) THALES 2000-2004 All rights reserved
#* This file and the information  it contains  are   property  of  THALES  and
#* confidential.   They  shall  not be reproduced nor disclosed  to any person
#* except to those having  a need  to know them without  prior written consent
#* of THALES.
#* =========================================================================== *
#===============================================================================


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

# CARDAMOM installation folder
CDMW_HOME="`cat $SITE_CONFIG | grep CDMW_HOME |cut -d= -f2`"

# We determine the target
HOST_TYPE="`$CDMW_HOME/bin/config.guess`"

shift

exec $DIRECTORY/../$HOST_TYPE/$PROGRAM $*
