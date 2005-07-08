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

TESTS_FILE="../data/CdmwIntegrationTests.output"

rm -f $TESTS_FILE

site_file=$CDMW_RUNTIME_ROOT/site.mk
CXX_VER="`cat $site_file | grep CXX_VER | cut -d= -f2`"
CXX_NAME="`cat $site_file | grep CXX_NAME | cut -d= -f2`"
ORB_CPP="`cat $site_file | grep ORB_CPP | cut -d= -f2`"
PLATFORM="`$CDMW_RUNTIME_ROOT/bin/config.guess`"
LD_LIBRARY_PATH=$CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$PLATFORM-$CXX_NAME-$CXX_VER:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

# Change to the exec dir
OLD_DIR="`pwd`"
cd ../exec

trap 'echo "Signal trapped, stop command send to platform daemon."; $CDMW_RUNTIME_ROOT/integration/exec/cdmw_platform_daemon.sh stop; exit' 0 2
./cdmw_platform_daemon.sh 2>&1 | tee $TESTS_FILE

# Come back to the previous dir
cd $OLD_DIR

