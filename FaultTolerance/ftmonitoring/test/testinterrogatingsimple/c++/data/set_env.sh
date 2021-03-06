#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

# for manual launch : CDMW_ROOT must be set
export PRODDIR=`find ../delivery/bin/* -type d -print | xargs basename`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CDMW_ROOT/Foundation/common/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/Foundation/osthreads/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/Foundation/ossupport/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/Foundation/orbsupport/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/Foundation/testutils/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/FaultTolerance/idllib/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/FaultTolerance/ftmonitoring/common/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/FaultTolerance/ftmonitoring/simplemonitoring/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/FaultTolerance/ftmonitoring/mutualmonitoring/c++/delivery/lib/$PRODDIR:$CDMW_ROOT/FaultTolerance/ftmonitoring/c++/delivery/lib/$PRODDIR

export PATH=$CDMW_ROOT/FaultTolerance/ftmonitoring/test/testinterrogatingsimple/c++/delivery/bin/$PRODDIR:$PATH

