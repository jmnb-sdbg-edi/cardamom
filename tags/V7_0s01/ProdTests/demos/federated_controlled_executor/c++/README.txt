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

Welcome to the CDMW DEMO
=========================

The purpose of CDMW DEMO is to provide an comprehensive example for writing
software based on CDMW runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CDMW users.

This particular demo is intended to provide a way to control federated
executors interactively.
This demo runs 3 triples of processes to achieve its aim; each triple differs
from the others because of its location: a triple-specific port is chosen for
each triple, different from the other triple-specific ports of the other triples.
The processes composing each triple are the following:
1. TimeoutHandler to provide task execution when timer expires,
2. TimeReader to monitor the current time of the timer of process 1;
   the current time is printed every 5 seconds,
3. TimeAdmin to manage the timer using commands set time/rate, pause and
   resume,

The main feature this demo is intended to demonstrate is that any change in
a timer (of a triple) made through commands set time/rate, pause and resume
specified from that triple's TimeAdmin, affect all the timers propagating
the change occurred to all of them.

To build CDMW demo:
	make all

To run CDMW demo:
	cd run
	start_demo.sh <policy> <display> <port1> <port2> <port3>

Where: <policy>  can be CANCELL_ALL|ENFORCE_INTERVAL|ENFORCE_DEADLINE
       <display> specifies the display environment in the format <hostname>:0.0
       <port1>   specifies the port used for the 1-st timeout handler, time reader and time admin
       <port2>   specifies the port used for the 2-nd timeout handler, time reader and time admin
       <port3>   specifies the port used for the 3-rd timeout handler, time reader and time admin

CONTENT:
=======
Makefile      : Global makefile
README.txt    : This file
config.mk     : CDMW configuration file for use by demo makefiles
site.mk       : Configuration file used by demo makefiles
data/         : Contains XML data files for starting demo programs
idl/          : Contains 'user-provided' IDL files
include/      : Contains 'user-provided' header files
run/          : Contains scripts for starting demo programs
src/          : Contains 'user-provided' source files
xml/          : Contains 'user-provided' XML data files for CDMW codegeneration

The following directories are created by the global makefile
generated/    : Contains CDMW generated files
host-type/    : Build directory

