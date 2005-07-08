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
step whatever the CARDAMOM packages selected.
This means that it is not possible to compile and execute all the tutorials
depending on the CARDAMOM configuration selected when compiling CARDAMOM.



First of all, configure the tutorial environment:
  > ./configure.sh

To execute a Tutorial :
  execute "make" to generate executable; for instance
  > cd hello
  > make
  
  then go to "run" directory of example and type "start.sh" to run example
  > cd run
  > start.sh


CONTENT:
=======
ORBacus410.mk : ORBacus/C++ 4.x configuration file
tao13.mk      : ACE/TAO 1.3.x configuration file
README.txt    : This file
config.mk     : CARDAMOM configuration file for use by tutorial Makefiles
configure.sh  : Shell script to generate site.mk file
site.mk       : Configuration file used by tutorial Makefiles (generated file)


For each tutorial subdirectory :

    Makefile      : Global Makefile
    data/         : Contains XML data files for starting tutorial programs
    idl/          : Contains 'user-provided' IDL files
    include/      : Contains 'user-provided' header files
    run/          : Contains scripts for starting tutorial programs
    src/          : Contains 'user-provided' source files
    xml/          : Contains 'user-provided' XML data files for CARDAMOM codegeneration

    The following directories are created by the global Makefile
    generated/    : Contains CARDAMOM generated files
    host-type/    : Build directory


examples :

polynome_lifecycle3 : 2nd degree polynome, check roots interval, using Cardamom factories
                      (no roots, single root, two roots). 
                      Server provides polynome (request coefs) to client. 
                      Server builds the object id requested by factory without client
                      parameter.   
                                                     
printerBW_lifecycle2 : print on Black and White printer, using Cardamom factories
                      Server creates Black and White printer
                      Client provides device name parameter to server.
printerColor_lifecycle2 : 
                      print on Color printer, using Cardamom factories
                      Server creates Color printer
                      Client provides device name parameter to server.
                      Interface "set_color" provided in idl to set printer in color.
                      The client of printerBW_lifecycle2 calls the Color printer server
                      still having the capacity to print in black and white (object inheritance)
printerColor_lifecycle3 : 
                      print on Color printer, using Cardamom factories
                      Server creates Color printer set to print in color or in B&W 
                      according to factory interface called (factory inheritance is used)                      
                      Client provides device name parameter to server.
                      Interface "set_color" provided in idl to set printer in color.
                      The client of printerBW_lifecycle2 calls the Color printer server
                      still having the capacity to print in black and white (object inheritance)
                      
                      
hello               : basic exemple with client calling server using file object reference
                      Cardamom ORB_init usage
                      
namingAndRepository : in server, registering an object in cardamom repository using domain of 
                      cardamom default root context
                      in client, get object reference from cardamom repository using naming interface 
                      
naming_objectBase   : in server initialise a base of object in repository (using domain)
                      TUTORIAL/HELLO/US/HelloInterface (John)
                      TUTORIAL/HELLO/US/DENVER/HelloInterface (Mike)
                      TUTORIAL/HELLO/US/DALLAS/HelloInterfaceBob
                      TUTORIAL/HELLO/US/DALLAS/HelloInterfacePeter
                      TUTORIAL/HELLO/US/DALLAS/HelloInterfaceJack
                      
                      in client (using naming interface)
                      get the HelloInterface object from TUTORIAL/HELLO/USA/HelloInterface
                      get the HelloInterface object from TUTORIAL/HELLO/USA/DENVER/HelloInterface
                      get the HelloInterface objects from TUTORIAL/HELLO/USA/DALLAS using two list methods
                      (.list and .list_context)
                      
naming_domLink      : in server initialise a base of objects in repository (using domain)
                      TUTORIAL/HELLO/USA/NEW_YORK/HelloInterface (John)
                      TUTORIAL/HELLO/USA/DETROIT/HelloInterface (Mike)
                      Under the domain TUTORIAL/HELLO/AMERICA, create a new domain USA linked to 
                      the domain TUTORIAL/HELLO/USA and its corresponding context
                      
                      in client (using naming interface)
                      get the HelloInterface (John) object from TUTORIAL/HELLO/AMERICA/USA/NEW_YORK/HelloInterface
                      get the HelloInterface (Mike) object from TUTORIAL/HELLO/AMERICA/USA/DETROIT/HelloInterface
                      

naming_write        : in server initialise a base of object in repository (using domain)
                      TUTORIAL/HELLO/US/HelloInterface (John)
                      
                      in client (using naming interface)
                      try to bind a new HelloInterface (Mike) under TUTORIAL/HELLO/US domain
                      (not permitted operation)
                      try to bind a new HelloInterface (Mike) under admin root context
                      (permitted operation)
                      
naming_writeExt     : in server initialise a base of object in repository (using domain)
                      TUTORIAL/HELLO/US/HelloInterface (John)
                      
                      in client (using naming interface)
                      try to bind a new HelloInterface (Mike) under default root context
                      (permitted operation)

trace               : in server initialise the trace library and trace a message to 
                      the started trace collector
                      
supervision         : start and stop the unmanaged process "trace" used in trace exemple
                      with the repository and trace services
                      
supervisionMngt0    : the server is a process managed by the supervision, 
                      using platformMgt interface
                      start and stop the managed process

supervisionMngt1    : the server is a process managed by the supervision, 
                      using platformMgt interface and trace
                      start and stop the managed process
                      
supervisionMngt2    : the server is a process managed by the supervision, 
                      using CdmwInit::ProcessControl and CdmwInit::InitUtils::init_platform_interface
                      start and stop the managed process
                      
supervisionMngt2Dbug: as supervisionMngt2 but the server is started under the debugger ddd/gdb 
                      to debug in code. Breaks has been set on the methods 
                      on_initialise() and on_run() of HelloServerProcessControl by the script helloServer.sh,
                      this script builds a commmand file for ddd
                      use stop_system.sh to stop the processes by the supervision
                      
supervisionMngt3    : example of user event notification and observation.

supervisionMngt4    : example of a Cardamom Supervision Administration Console
                      and a Cardamom Supervision Observer.

init_generationCode : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      no factory used
                      Client calls display_hello of HelloInterface implemented in server                   
                      
lifeCycleEntity     : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      Cardamom Entity factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server
                      
lifeCycleEntity_property     
                    : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      property service is started
                      Cardamom Entity factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server
                      Server is stopped then restarted to check object persistance
                      A second HelloInterface object is created during initialization 
                      then removed when Server is restarted
                      
lifeCycleSession    : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      Cardamom Session factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server
                      
lifeCycleService    : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      Cardamom Service factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server

lifeCycleUnmanagedEntity : 
                      use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      Cardamom UnmanagedEntity factory is generated to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server
                      
lifeCycleUserFac    : use code generation tool to generate Cardamom init
                      define Server and Client
                      repository and trace services are used
                      user factory is used to create by client HelloInterface object
                      Client calls display_hello of HelloInterface implemented in server                  

thread              : same as lifeCycle example, but client is implemented in a thread 
                      created on initialisation supervision step and started on run 
                      supervision step. When running client print periodically "Hello". 
                      
event               : Client thread sends periodically a non typed event to server
                      use event service of ORB
                      use code generation tool to generate Cardamom init

eventTyped          : same as event but in this case the event are typed as defined 
                      in idl file    (Only for Orbacus)
                                                  

lb_initialization   : Illustrates how to the Load Balancing Service is used and how to define LB Strategies.

lb_demo             : Illustrates how to the Load Balancing Service is used and the how the load balancing group
                      is updated when a member crashes.

clock

controlled_clock

controlled_executor_clock
