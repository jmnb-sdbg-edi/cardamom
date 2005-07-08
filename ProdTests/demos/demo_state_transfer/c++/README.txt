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

#=================================#
#      State Transfer Demo        #
#=================================#



Description of the Demo
-----------------------

The purpose of this demo is to provide a comprehensive example of use of the 
state transfer library for fault tolerance.

A group of 3 fault tolerance servers are running on 3 hosts. They implement a
Database IDL interface which provides 2 operations, get_data() and set_data():

    // IDL
    interface Database {
        Data get_data(in Ident id)
            raises (DataNotFound);
        void set_data(in Data d);
    };

The stored Data is a structure containing an identifier and 2 sub-structures:
    // IDL
    struct DataPart1 {
        short version;
        short sh;
    };
    
    struct DataPart2 {
        short version;
        unsigned long ul;
    };

    struct Data {
        Ident id;  
        DataPart1 part1;
        DataPart2 part2;
        DataPart1 part3;
    };
    
The Database stores a Data in 2 DataStores, 1 for each sub-structure.
The DataStores of the primary Database have to transfer their 
sub-structures to the DataStores of the backup Databases, each time a 
sub-structure is modified.

A client get the Database group reference and use it to invoke operations
on the Database. It makes the following scenario 20 times:
 - get the previous Data from Database
 - check that the Data values are the same than previously
 - modify the Data with new values 
 - set it into Database 
 - wait 3 seconds

The 3 seconds delay could be used to kill the primary Database. Thus, the 
client's get_data() is automatically forwarded to a new primary Database,
and the client should receive an up-to-date Data. 


Note
----
To be able to encode/decode the data of DataStores for state transfer,
the data required to be defined as a data_traits. As both sub-structures
are fixed-length struct, the following macros are used into Database_impl.hpp:
  DEFINE_FIXED_LENGTH_DATA_TRAITS(Demo::DataPart1);
  DEFINE_FIXED_LENGTH_DATA_TRAITS(Demo::DataPart2);
If a variable-length struct or a sequnce is used, the following macro 
should be used:
  DEFINE_VARIABLE_LENGTH_DATA_TRAITS(Demo::MyVariableStruct);
For CORBA object references, no macro use is required. But the data type
declared into the Datastore template should be the stub class, not the 
generated _var class. 
  


Exact scenario of the Demo
--------------------------

1) The Cardamom Repository is started into a first application (CdmwServices).
2) Then the second application (FTApplication) is started with the following
   processes:
    2.1) The group_creator: it create the object group references for
         futur Database objects, and register it into the Repository.
    2.2) The 3 databeses (one on each host): they get the object group
         reference from the Repository, and add their Database object as 
         member of this group.
    2.3) The client: It get the object group reference from the Repository
         and use it for method invocations, in the same way that with a 
         normal object reference.



How to launch the demo
----------------------
Into the 'run' directory of the demonstration, type './startDemo.sh'.

This script starts 3 daemons on the 3 hosts (via rsh), starts the FTManager,
starts the supervision, and launches the scenario of the demonstration. 
The 3 databases are running in separate windows. Thus, you can kill a database
at any moment to see that the client is redirected on another database without
impact.


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




