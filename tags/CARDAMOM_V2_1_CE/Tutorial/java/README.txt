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

Welcome to the CARDAMOM TUTORIAL Version 2.0
============================================

The purpose of CARDAMOM Tutorial is to provide comprehensive examples for writing
software based on CARDAMOM runtime middleware. Provided Makefiles may be reused
to write more sofisticated ones that suit needs of CARDAMOM users.

Note
====

This tutorial is designed to be used with Version 1.1 of Cardamom

------------------------------------------------------------------------------------

The whole source code of tutorials is installed during the CARDAMOM installation
step whatever the CDMW packages selected.
This means that it is not possible to compile and execute all the tutorials
depending on the configuration selected when compiling CARDAMOM.

First of all, configure the tutorial environment:
  > ./configure.sh

To execute a Tutorial :
  execute "make" to generate executable
  > cd hello
  > make
  
  then go to "run" directory of example and type "start.sh" to run example
  > cd run
  > start.sh


CONTENT:
=======
ORBacus410.mk : ORBacus/Java 4.x configuration file
README.txt    : This file
config.mk     : CDMW configuration file for use by tutorial makefiles
configure.sh  : Shell script to generate site.mk file
site.mk       : Configuration file used by tutorial makefiles (generated file)


For each tutorial subdirectory :

    Makefile      : Global makefile
    data/         : Contains XML data files for starting tutorial programs
    idl/          : Contains 'user-provided' IDL files
    run/          : Contains scripts for starting tutorial programs
    src/          : Contains 'user-provided' source files
    xml/          : Contains 'user-provided' XML data files for CDMW codegeneration

    The following directories are created by the global makefile
    generated/    : Contains CDMW generated files
    host-type/    : Build directory


examples :

hello               : basic exemple with client calling server using file object reference
                      Cdmw ORB_init usage
                      
namingAndRepository : in server, registering an object in cdmw repository using domain of 
                      cdmw default root context
                      in client, get object reference from cdmw repository using naming interface 
                      
trace               : in server initialise the trace library and trace a message to 
                      the started trace collector
                      
supervision         : start and stop the unmanaged process "trace" used in trace exemple
                      with the repository and trace services
                   
supervisionMngt1    : the server is a process managed by the supervision, 
                      using platformMgt interface and trace
                      start and stop the managed process
                      
supervisionMngt2    : the server is a process managed by the supervision, 
                      using cdmwinit.ProcessControl and cdmwinit.InitUtils.initPlatformInterface
                      start and stop the managed process
                      
init_generationCode : use code generation tool to generate Cdmw init
                      define Server and Client
                      repository and trace services are used
                      no factory used
                      Client calls display_hello of HelloInterface implemented in server                   
                      
lifeCycleService    : use code generation tool to generate Cdmw init
                      define Server and Client
                      repository and trace services are used
                      Cdmw Service factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server

thread              : same as lifeCycle example, but client is implemented in a thread 
                      created on initialisation supervision step and started on run 
                      supervision step. When running client print periodically "Hello". 
                      
event               : Client thread sends periodically a non typed event to server
                      use event service of ORB
                      use code generation tool to generate Cdmw init
                                                  
