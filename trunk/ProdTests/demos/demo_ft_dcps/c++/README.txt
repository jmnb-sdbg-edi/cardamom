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

#
#
#=========================================================================

Welcome to the CDMW DEMO
=========================

The purpose of CDMW DEMO is to provide an comprehensive example for writing
software based on CDMW runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CDMW users.

This demo is a Fault Tolerant application with a state logging ans recovery based on DCPS provided by Splice 2.
Executables are run on two hosts. This mandatory to create an object group reference used by CORBA FT (Fault Tolerance).

To build CDMW demo ft dcps:
   export HOSTNAME2=<remote_host_name>
   Set PATH and LD_LIBRARY_PATH to use Splice lib
   make all

Running the demo:
	cd run
	./start_demo.sh
	=> wait for Launch.sh windows and tape Ctrl-C inside hello1 when displaying:
	  hello1 : <####### HELLO 3  ######>

	  hello2 will immediatly recover hello1 state using DCPS and go on displaying 
     hello2 : <####### HELLO 4  ######>

To have view on splice activity you could use asc -C $SPLICE/etc -Splice-port=21920..

Note: If the platform daemon on the remote host exits immediately then
ensure that the LD_LIBRARY_PATH has pointers to the ORB lib directory,
to Xerces C++ lib directory of that system and to Splice lib directory.


CONTENT:
=======
Makefile      : Global makefile
README.txt    : This file
config.mk     : CDMW configuration file for use by demo makefiles
tao13.mk      : TAO 1.3.x configuration file
site.mk       : Configuration file used by demo makefiles
data/         : Contains XML data files for starting demo programs
idl/          : Contains 'user-provided' IDL files
include/      : Contains 'user-provided' header files
run/          : Contains scripts for starting demo programs
src/          : Contains 'user-provided' source files
xml/          : Contains 'user-provided' XML data files for CDMW codegenration


The following directories are created by the global makefile
generated/    : Contains CDMW generated files
<host-type>/  : Build directory

Technical considerations on daemon & faultmanager conf files
------------------------------------------------------------
At the time of this writing (1.12.04) most of the demos (like this one) have
their own CdmwPlatformMngtDaemon_conf, but we could use the file defined in
share directory.
Advantage to have localy: we can define specific adresses to automaticaly
avoid problems whith another test/demo process launched by another one (for
example i use last digit of id and pwd length to build adresses in Makefile).
In addition there is no need to modify port configuration in Scenario file if
the share file of the daemon conf has changed (or in the start script if
ftmanager conf port has changed)..
Advantage to get in share directory: evolutions on the configuration file
fields are automaticaly kept + these delivered files are tested..
For the moment i have choosed to keep local where it is already local (and so
to add a local CdmwFaultToleranceManager_conf) and to take the share conf
files when they are not local.

