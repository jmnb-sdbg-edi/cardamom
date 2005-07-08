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

The purpose of CDMW DEMO is to provide a comprehensive example for writing
software based on CDMW runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CDMW users.

This demo is a Fault Tolerant CCM application with a state logging and recovery
based on DCPS provided by Splice 2. This is an optimistic approach for state
replication for we assume that the Splice 2 daemon will not fail during data
publishing. We cannot state for sure that data is published on the network for
DCPS is inherently asynchronous.
Components are assembled manually and object groups are created by a dedicated
group creator process.

This demo can run on single host for we choose to run replicas on different 
CDMW processes on the same application. (use of --ReplicationLevel=LOOSE for 
the FTManager). The splice daemon and libraries use shared memory for their 
communication and do not initialise the network since we do not provide
-Splice_port argument.

NOTA: If you require the use of the network, add a -Splice_port argument to all 
      Splice daemons and executables and to the manual assembly program. The latter
      will follow up the information to server components to initialise their
      Splice 2 objects.

To enable intialisation of splice in the components, configuration attributes 
have been added to get some important information needed by splice. Splice normally
gets it using argc, argv; but these are no longer available to components.

NOTE on the use of request duration time.
The request duration time is the time where a request is considered to be valid 
and the FT infrastructure is allowed to re-send again to the same target or to
an alternate object if the first one fails. If however the request duration time has
expired the FT infrastructure will not resend it again as it may be executed twice
by the target object.
The request duration time uses UTC time as defined in X/Open specification. So
its value is in hendreds of nanoseconds. A value of 50,000,000 for the request
duration time refers to only 5 seconds.


DEMO Description:
The data/Scenario.xml file contains all entities involved in this demo. The 
group_creator executable creates object group references (IOGR) for both the 
component interface and the server_facet port through the CDMW Replication
Manager. These are registered within the Naming Service (CDMW Naming And
Repository) in the 'demo_ccm_fr/hello_servers' naming context.
The massebly executable is the program performing the creation of the components. 
It configures the server components, make the components and their facets join 
their respective object groups and then connect the facet group reference to the
client's receptacle.


 "MyNaming/Tutorial/HelloClient"            "MyNaming/Tutorial/HelloServer1"
       O                                                O................
       |                                                |               .
+------+-----+                                   +------+-----+         . 
|   Client   |                                   |   Server   |         . 
|            | client_receptacle    server_facet |            |         . 
|            +------( .................()--------+            |         .
|            |                   .               |            |         .
|            |                   .               |            |         .
|            | to_naming         .               |            |         . 
|            +------(            .               |            |         . IOGR 
|            |                   .          +----+------+     |         .<---+
|            |                   .          | ServerNo1 |     |         .    |
|            |                   .          +-----------+     |         .    |
|            |                   .          | sever     |     |         .    |
|            |                   .          +-----------+     |         .    |
|            |          IOGR     .          | ""        |     |         .    |
|            |          +------> .          +----+------+     |         .    |
+------------+         /         .               +------------+         .    |
                      /          .                                      .    |
 "demo_ccm_ft/hello_servers/hello_server_facet.group"                   .    |
                                 .                                      .    |
                                 . "demo_ccm_ft/hello_servers/hello_server.group"
                                 .                                      . 
                                 .                                      . 
                                 .                                      . 
                                 .                                      . 
                                 .          "MyNaming/Tutorial/HelloServer2" 
                                 .                      O................
                                 .                      |               .
                                 .               +------+-----+         .
                                 .               |   Server   |         .
                                 .  server_facet |            |         .
                                 ......()--------+            |         .
                                 .               |            |         .
                                 .               |            |         .
                                 .               |            |         .
                                 .               |            |         .
                                 .          +----+------+     |         .
                                 .          | ServerNo2 |     |         .
                                 .          +-----------+     |         .     
                                 .          | sever     |     |         .     
                                 .          +-----------+     |         .     
                                 .          | ""        |     |         .     
                                 .          +----+------+     |         .
                                 .               +------------+         .
                                 .                                      .  
                                 .          "MyNaming/Tutorial/HelloServer3"         .
                                 .                      O................
                                 .                      |          
                                 .               +------+-----+          
                                 .               |   Server   |          
                                 .  server_facet |            |          
                                 ..... ()--------+            |          
                                                 |            |
                                                 |            |
                                                 |            |
                                                 |            |
                                            +----+------+     |
                                            | ServerNo3 |     |
                                            +-----------+     |               
                                            | sever     |     |               
                                            +-----------+     |               
                                            | ""        |     |               
                                            +----+------+     |
                                                 +------------+


To build CDMW demo cmm ft:
        Set PATH and LD_LIBRARY_PATH to use Splice lib
	make all

Running the demo:
	cd run
	./start_demo.sh
        => Wait for the message saying you can 'crash' the server during request procesing
           and type Ctrl-C. This will redirect the request to an alternative server.
        The request will still be redirected to an alternate server if toy crash the server
        outside of the request processing ;-)  


To have view on splice activity you could use asc -C $SPLICE/etc ..

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
At the time of this writing (1.12.04) most of the demos (like this one) have their own CdmwPlatformMngtDaemon_conf, but we could use the file defined in share directory.
Advantage to have localy: we can define specific adresses to automaticaly avoid problems whith another test/demo process launched by another one (for example i use last digit of id and pwd length to build adresses in Makefile). In addition there is no need to modify port configuration in Scenario file if the share file of the daemon conf has changed (or in the start script if ftmanager conf port has changed)..
Advantage to get in share directory: evolutions on the configuration file fields are automaticaly kept + these delivered files are tested..
For the moment i have choosed to keep local where it is already local (and so to add a local CdmwFaultToleranceManager_conf) and to take the share conf files when they are not local.
