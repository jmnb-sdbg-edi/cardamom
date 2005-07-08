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


if test ! -x `which $TERM`
then
    TERM=xterm
fi

DIRECTORY="`dirname $0`"
PROGRAM="`basename $0 .sh`"
export PROGRAM

SITE_CONFIG="$DIRECTORY/../site.mk"

# SPLICE installation folder
SPLICE_BIN_PATH="`cat $SITE_CONFIG | grep SPLICE_BIN_PATH | cut -d= -f2`"
export SPLICE_BIN_PATH

#. $SPLICE_BIN_PATH/../release.com
#exec $TERM -ls -sb -sl 500 -e $SPLICE_BIN_PATH/$PROGRAM $*
#$SPLICE_BIN_PATH/$PROGRAM $*

start() {
	echo "Starting Splice daemon..."
	. $SPLICE_BIN_PATH/../release.com
#exec $TERM -ls -sb -sl 500 -e $SPLICE_BIN_PATH/$PROGRAM $*
	exec $SPLICE_BIN_PATH/$PROGRAM $*
}


stop() {
	echo "Stopping Splice..."
	$SPLICE_BIN_PATH/stop_splice
}

# See how we were called.
case "$1" in
  start)
		shift
        start $*
        ;;
  stop)
        stop
        ;;
  *)
        echo $"Usage: $PROGRAM {start|stop}"
        exit 1
esac
