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

Welcome to the Cdmw.CCM320 Tutorial 
=========================

The CCM320 tutorial provides an running exemple for using TAO's Load Balancing
service to access CORBA components.
The LoadBalancing strategy in use is session oriented and non-adaptive. We choose
the use of RoundRobin but the same code/example holds for other strategies
supported bu TAO LoadManager.

This demo is a CCM application with 3 stateless server components each providing 
a facet with a single operation called 'display_hello()'.
The client.sh program launches 10 client programs (Cl-A to Cl-J) and each one
of them acquires the facet's group reference from the Naming Service (CARDAMOM
Naming And Repository service) and calls display_hello() operation.
Each component display identity of the caller on a separate window. We can then
see how requests are delivered to the 3 server components.
Components are assembled manually and object groups are created by a dedicated
group creator process.

This demo can run on single host for we did not require the use of a LoadMonitor
object as we are using a non-adaptive strategy.

DEMO Description:
The data/Scenario.xml file contains all entities involved in this demo. The 
lb_group_creator executable creates object group references (IOGR) for both the
component interface and the server_facet port through the TAO LoadManager object.
These are registered within the Naming Service (CARDAMOM Naming And Repository) in
the 'demo_ccm_lb/hello_servers' naming context.
The massebly executable is the program performing the creation of the components. 
It configures the server components and then make the components and their facets
join their respective object groups.
The client.sh shell script runs 10 client processes sequentially.

10 clients calling a display_hello() each.

           O                              "MyNaming/Tutorial/HelloServer1"
         -O|--                                          O................
        -O|--                                           |               .
       -O|--\                                    +------+-----+         . 
      -O|--\                                     |   Server   |         . 
     -O|--\    display_hello()      server_facet |            |         . 
    -O|--\     ..................+---->()--------+            |         .
   -O|--\                        |               |            |         .
  -O|--\                         |               |            |         .
 -O|--\                          |               |            |         . 
--|--\                           |               |            |         . IOGR 
  | \                            |          +----+------+     |         .<---+
 / \                     IOGR    |          | ServerNo1 |     |         .    |
                        +------> |          +----+------+     |         .    |
                       /         |               +------------+         .    |
                      /          |                                      .    |
 "demo_ccm_lb/hello_servers/hello_server_facet.group"                   .    |
                                 |                                      .    |
                                 | "demo_ccm_lb/hello_servers/hello_server.group"
                                 |                                      . 
                                 |                                      . 
                                 |                                      . 
                                 |                                      . 
                                 |          "MyNaming/Tutorial/HelloServer2" 
                                 |                      O................
                                 |                      |               .
                                 |               +------+-----+         .
                                 |               |   Server   |         .
                                 |  server_facet |            |         .
                                 +---->()--------+            |         .
                                 |               |            |         .
                                 |               |            |         .
                                 |               |            |         .
                                 |               |            |         .
                                 |          +----+------+     |         .
                                 |          | ServerNo2 |     |         .
                                 |          +----+------+     |         .
                                 |               +------------+         .
                                 |                                      .  
                                 |          "MyNaming/Tutorial/HelloServer3"         .
                                 |                      O................
                                 |                      |          
                                 |               +------+-----+          
                                 |               |   Server   |          
                                 |  server_facet |            |          
                                 +---->()--------+            |          
                                                 |            |
                                                 |            |
                                                 |            |
                                                 |            |
                                            +----+------+     |
                                            | ServerNo3 |     |
                                            +----+------+     |
                                                 +------------+


To build CARDAMOM demo cmm ft:
	make all

Running the demo:
	cd run
	./start.sh



CONTENT:
=======
Makefile      : Global Makefile
README.txt    : This file
config.mk     : CARDAMOM configuration file for use by demo Makefiles
tao13.mk      : TAO 1.3.x configuration file
site.mk       : Configuration file used by demo Makefiles
data/         : Contains XML data files for starting demo programs
idl/          : Contains 'user-provided' IDL files
include/      : Contains 'user-provided' header files
run/          : Contains scripts for starting demo programs
src/          : Contains 'user-provided' source files
xml/          : Contains 'user-provided' XML data files for CARDAMOM codegenration

The following directories are created by the global Makefile
generated/    : Contains CARDAMOM generated files
<host-type>/  : Build directory


