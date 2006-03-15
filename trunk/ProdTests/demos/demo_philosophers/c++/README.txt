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

Welcome to the CDMW PHILOSOPHERS DEMO
=====================================

The purpose of CDMW PHILOSOPHERS DEMO is to provide an comprehensive example
for writing software based on CDMW runtime middleware. Provided Makefiles may
be reused to write more sofisticated ones that suit needs of CDMW users.

To build CDMW demo philosophers:
	make all

To run the demo with C++ components only:
	cd run
	./start_demo.sh

To run the first demo with C++ and Java components together
(C++: Philosophers, Java: Observer):
    (cd ../java && make all)
    cd run
    ./make_assembly_package_2.sh
    ./start_demo_2.sh

To run the second demo with C++ and Java components together
(C++: Observer, Java:Philosophers):
    (cd ../java && make all)
    cd run
    ./make_assembly_package_3.sh
    ./start_demo_3.sh


CONTENT
=======

Makefile      : Global makefile
ORBacus4.mk   : ORBacus/C++ 4.x configuration file
tao13.mk      : TAO 1.3.x configuration file
tao14-OF.mk   : TAO 1.4.x OpenFusion configuration file
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


Technical considerations about the user environnement
-----------------------------------------------------
At the time of this writing (25.11.04) most of the demos have their own
CdmwPlatformMngtDaemon_conf. But not this one : we use the share directory.
Advantage to have localy: we can define specific adresses to automaticaly
avoid problems whith another test/demo process launched by another one
(for example i use last digit of id and pwd length to build adresses in
Makefile).
To do this in our case (not local), modify the ports in files of share
directory, then touch run/start_daemon.sh.in, touch
data/integ_deployment.xml.in, and do a "make data".
Advantage to get in share directory: evolutions on the configuration file
fields are automaticaly kept + these delivered files are tested.
For the moment i have choosed to keep local where it is already local (and
so to add a local CdmwFaultToleranceManager_conf) and to take the share conf
files when they are not local.
