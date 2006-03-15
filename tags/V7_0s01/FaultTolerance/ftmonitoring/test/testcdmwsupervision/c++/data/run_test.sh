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

arg1=--MUTUAL_PULL_CORBALOC
arg2=corbaloc:miop:1.0@1.0-testmonitoringlib-3/225.1.1.8:5000
arg3=--MUTUAL_PUSH_CORBALOC
arg4=corbaloc:miop:1.0@1.0-testmonitoringlib-4/225.1.1.8:5001
arg5=--SIMPLE_PULL_CORBALOC
arg6=corbaloc:miop:1.0@1.0-testmonitoringlib-1/225.1.1.8:4998
arg7=--SIMPLE_PUSH_CORBALOC
arg8=corbaloc:miop:1.0@1.0-testmonitoringlib-2/225.1.1.8:4999
# HOST_PRIORITY is default ELECTION_STRATEGY
#arg9=--ELECTION_STRATEGY
#arg10=HOST_PRIORITY

#testCdmwSupervision $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8 --MASTER_INTERROGATION 9000 --SLAVE_ACKNOWLEDGEMENT 8000 --MASTER_CONFIRMATION 7000 --MASTER_ELECTION 6000 --DAEMON_ACKNOWLEDGEMENT 5000 $*
testCdmwSupervision $arg1 $arg2 $arg3 $arg4 $arg5 $arg6 $arg7 $arg8 $*
