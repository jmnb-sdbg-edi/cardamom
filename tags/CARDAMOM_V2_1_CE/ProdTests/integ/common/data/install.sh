#!/bin/sh
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

cd ../..
CDMW_RUNTIME_ROOT=`pwd`

INTEG_DATA_DIR=$CDMW_RUNTIME_ROOT/integration/data
INTEG_EXE_DIR=$CDMW_RUNTIME_ROOT/integration/exec
INTEG_EXE_BUILD=$CDMW_RUNTIME_ROOT/integration/build

echo "Creating the exec directory"
rm -rf $INTEG_EXE_DIR 
mkdir $INTEG_EXE_DIR 

# We determine the site.mk variables
ORB_CXX="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
ORB_ADA="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_ADA |cut -d= -f2`"

ADA_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ADA_NAME |cut -d= -f2`"
ADA_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ADA_VER |cut -d= -f2`"

CXX_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
CXX_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"

HOST_TYPE="`$CDMW_RUNTIME_ROOT/bin/config.guess`"

CXX_BINARY_TYPE=$HOST_TYPE-$CXX_NAME-$CXX_VER
CXX_EXE_DIR="prod/$ORB_CXX/$CXX_BINARY_TYPE"

ADA_BINARY_TYPE=$HOST_TYPE-$ADA_NAME-$ADA_VER
ADA_EXE_DIR="prod/$ORB_ADA/$ADA_BINARY_TYPE"

ORBACUS_BIN_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_BIN_PATH |cut -d= -f2`"

echo "Installing data"

#
# README
# the repository and its test exe are used for the integration tests of the
# platform management
#

cp $CDMW_RUNTIME_ROOT/platformdaemon/c++/src/*.dtd $INTEG_DATA_DIR
cp $CDMW_RUNTIME_ROOT/platformadmin/c++/src/*.dtd $INTEG_DATA_DIR

cp $CDMW_RUNTIME_ROOT/namingandrepository/c++/src/*.dtd $INTEG_DATA_DIR

cp $CDMW_RUNTIME_ROOT/integration/build/c++/lifecycle/generated/Process_Desc.dtd $INTEG_DATA_DIR
cp $CDMW_RUNTIME_ROOT/integration/build/c++/lifecycle/data/*.xml $INTEG_DATA_DIR
cp $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test1/data/*.xml $INTEG_DATA_DIR/Cdmw_Gen_test1.xml
if [ -d $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test1/generated ]
then
   cp $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test1/generated/*.dtd $INTEG_DATA_DIR/Process_Desc_test1.dtd
fi
cp $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test2/data/*.xml $INTEG_DATA_DIR/Cdmw_Gen_test2.xml
if [ -d $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test2/generated ]
then
   cp $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test2/generated/*.dtd $INTEG_DATA_DIR/Process_Desc_test2.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/data/Server1.xml $INTEG_DATA_DIR/Ada_Server1.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/generated/*.dtd $INTEG_DATA_DIR/Ada_Server1_Desc.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/data/Server2.xml $INTEG_DATA_DIR/Ada_Server2.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/generated/*.dtd $INTEG_DATA_DIR/Ada_Server2_Desc.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/data/Client.xml $INTEG_DATA_DIR/Ada_Client.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/generated/*.dtd $INTEG_DATA_DIR/Ada_Client_Desc.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/data/Server1.xml $INTEG_DATA_DIR/Java_Server1.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/generated/*.dtd $INTEG_DATA_DIR/Java_Server1_Desc.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/data/Server2.xml $INTEG_DATA_DIR/Java_Server2.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/generated/*.dtd $INTEG_DATA_DIR/Java_Server2_Desc.dtd
fi
cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/data/Client.xml $INTEG_DATA_DIR/Java_Client.xml
if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/generated ]
then
   cp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/generated/*.dtd $INTEG_DATA_DIR/Java_Client_Desc.dtd
fi
# At this time no CPP integration test is written for the_validation_servers&client 
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/data/Server1.xml $INTEG_DATA_DIR/Server1.xml
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/generated/*.dtd $INTEG_DATA_DIR/Server1_Desc.dtd
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/data/Server2.xml $INTEG_DATA_DIR/Server2.xml
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/generated/*.dtd $INTEG_DATA_DIR/Server2_Desc.dtd
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/data/Client.xml $INTEG_DATA_DIR/Client.xml
#cp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/generated/*.dtd $INTEG_DATA_DIR/Client_Desc.dtd

#install dtd for FT
cp $INTEG_EXE_BUILD/c++/faulttolerance/test_server/generated/*.dtd $INTEG_DATA_DIR
cp $INTEG_EXE_BUILD/c++/faulttolerance/data/*.xml $INTEG_DATA_DIR
cp $INTEG_EXE_BUILD/c++/faulttolerance/data/*.in $INTEG_DATA_DIR

#install dtd files for platformmngt
cp $INTEG_EXE_BUILD/c++/platformmngt/data/*.xml $INTEG_DATA_DIR

# install dtd for ccm
cp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/softpkg.dtd $INTEG_DATA_DIR/softpkg.dtd
cp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/corbacomponent.dtd $INTEG_DATA_DIR/corbacomponent.dtd
cp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/componentassembly.dtd $INTEG_DATA_DIR/componentassembly.dtd
cp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/properties.dtd $INTEG_DATA_DIR/properties.dtd
cp $CDMW_RUNTIME_ROOT/ccmpackagemanager/data/CcmPackageManagerSave.dtd $INTEG_DATA_DIR/CcmPackageManagerSave.dtd

# install specific files for ccm integration test
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver1/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver1/generated/ComponentServer1.dtd $INTEG_DATA_DIR/ComponentServer1.dtd
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver1/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver1/generated/ComponentServer1.dtd $INTEG_DATA_DIR/ComponentServer1.dtd
   fi
fi
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver1/data/ComponentServer1.xml $INTEG_DATA_DIR/ComponentServer1.xml
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver2/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver2/generated/ComponentServer2.dtd $INTEG_DATA_DIR/ComponentServer2.dtd
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver2/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver2/generated/ComponentServer2.dtd $INTEG_DATA_DIR/ComponentServer2.dtd
   fi
fi
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver2/data/ComponentServer2.xml $INTEG_DATA_DIR/ComponentServer2.xml
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/clientcomponent/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/clientcomponent/generated/Basic_cif.idl $INTEG_DATA_DIR/Basic_cif.idl
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm/clientcomponent/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm/clientcomponent/generated/Basic_cif.idl $INTEG_DATA_DIR/Basic_cif.idl
   fi
fi

# install specific files for ccm event integration test
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/data/*.dtd $INTEG_DATA_DIR
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/data/*.xml $INTEG_DATA_DIR
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/data/*.conf $INTEG_DATA_DIR
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/GreekSchoolComponentServer/generated ] 
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/GreekSchoolComponentServer/generated/GreekSchoolComponentServer.dtd $INTEG_DATA_DIR
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/GreekSchoolComponentServer/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/GreekSchoolComponentServer/generated/GreekSchoolComponentServer.dtd $INTEG_DATA_DIR
   fi
fi
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/GreekSchoolComponentServer/data/GreekSchoolComponentServer.xml $INTEG_DATA_DIR
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/FrenchSchoolComponentServer/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/FrenchSchoolComponentServer/generated/FrenchSchoolComponentServer.dtd $INTEG_DATA_DIR
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/FrenchSchoolComponentServer/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/FrenchSchoolComponentServer/generated/FrenchSchoolComponentServer.dtd $INTEG_DATA_DIR
   fi
fi
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/FrenchSchoolComponentServer/data/FrenchSchoolComponentServer.xml $INTEG_DATA_DIR
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/ObserverComponentServer/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/ObserverComponentServer/generated/ObserverComponentServer.dtd $INTEG_DATA_DIR
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/ObserverComponentServer/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/ObserverComponentServer/generated/ObserverComponentServer.dtd $INTEG_DATA_DIR
   fi
fi
cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/ObserverComponentServer/data/ObserverComponentServer.xml $INTEG_DATA_DIR
if [ -d $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/philosophercomponent/generated ]
then
   cp $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/philosophercomponent/generated/Philo_cif.idl $INTEG_DATA_DIR/Philo_cif.idl
else
   if [ -d $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/philosophercomponent/generated ]
   then
      cp $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/philosophercomponent/generated/Philo_cif.idl $INTEG_DATA_DIR/Philo_cif.idl
   fi
fi

# create ccm_integration_commands.txt
cat $INTEG_DATA_DIR/ccm_integration_commands.txt.in | sed -e "s%@runtime_root@%$CDMW_RUNTIME_ROOT%g" > $INTEG_DATA_DIR/ccm_integration_commands.txt
cat $INTEG_DATA_DIR/java_ccm_integration_commands.txt.in | sed -e "s%@runtime_root@%$CDMW_RUNTIME_ROOT%g" > $INTEG_DATA_DIR/java_ccm_integration_commands.txt

# create ccm_event_integration_commands.txt
cat $INTEG_DATA_DIR/ccm_event_integration_commands.txt.in | sed -e "s%@runtime_root@%$CDMW_RUNTIME_ROOT%g" > $INTEG_DATA_DIR/ccm_event_integration_commands.txt
cat $INTEG_DATA_DIR/java_ccm_event_integration_commands.txt.in | sed -e "s%@runtime_root@%$CDMW_RUNTIME_ROOT%g" > $INTEG_DATA_DIR/java_ccm_event_integration_commands.txt


echo "Installing CDMW executables"

ln -s $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_launch_process.sh $INTEG_EXE_DIR/cdmw_platform_launch_process.sh
ln -s $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_kill_pid.sh $INTEG_EXE_DIR/cdmw_platform_kill_pid.sh
ln -s $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_kill_all_pids.sh $INTEG_EXE_DIR/cdmw_platform_kill_all_pids.sh
ln -s $CDMW_RUNTIME_ROOT/platformdaemon/c++/script/cdmw_platform_daemon.sh $INTEG_EXE_DIR/cdmw_platform_daemon.sh

ln -s $CDMW_RUNTIME_ROOT/platformdaemon/c++/$CXX_EXE_DIR/cdmw_platform_daemon $INTEG_EXE_DIR/cdmw_platform_daemon
ln -s $CDMW_RUNTIME_ROOT/platformagent/c++/$CXX_EXE_DIR/cdmw_platform_agent $INTEG_EXE_DIR/cdmw_platform_agent
ln -s $CDMW_RUNTIME_ROOT/platformsystem/c++/$CXX_EXE_DIR/cdmw_platform_supervision $INTEG_EXE_DIR/cdmw_platform_supervision
ln -s $CDMW_RUNTIME_ROOT/platformadmin/c++/$CXX_EXE_DIR/cdmw_platform_admin $INTEG_EXE_DIR/cdmw_platform_admin

ln -s $CDMW_RUNTIME_ROOT/namingandrepository/c++/$CXX_EXE_DIR/cdmw_naming_and_repository $INTEG_EXE_DIR/cdmw_naming_and_repository
ln -s $CDMW_RUNTIME_ROOT/namingandrepository/c++/test/test1/$CXX_EXE_DIR/cdmw_test_naming_and_repository $INTEG_EXE_DIR/cdmw_test_naming_and_repository
ln -s $ORBACUS_BIN_PATH/nameserv $INTEG_EXE_DIR/nameserv
ln -s $ORBACUS_BIN_PATH/irserv $INTEG_EXE_DIR/irserv
ln -s $INTEG_DATA_DIR/start_interface_repository.sh $INTEG_EXE_DIR/start_interface_repository.sh
ln -s $ORBACUS_BIN_PATH/eventserv $INTEG_EXE_DIR/eventserv
ln -s $CDMW_RUNTIME_ROOT/eventchannelmanager/c++/$CXX_EXE_DIR/cdmw_event_channel_manager $INTEG_EXE_DIR/cdmw_event_channel_manager

ln -s $CDMW_RUNTIME_ROOT/tracecollector/c++/$CXX_EXE_DIR/cdmw_trace_collector $INTEG_EXE_DIR/cdmw_trace_collector
ln -s $INTEG_EXE_BUILD/c++/naminginterface/$CXX_EXE_DIR/testNI $INTEG_EXE_DIR/testNI
ln -s $CDMW_RUNTIME_ROOT/naminginterface/ada/test/$ADA_EXE_DIR/testni $INTEG_EXE_DIR/ada_testNI
ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/lifecycle/$CXX_EXE_DIR/main $INTEG_EXE_DIR/main
ln -s $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test1/$CXX_EXE_DIR/HelloWorld $INTEG_EXE_DIR/CdmwGen_test1
ln -s $CDMW_RUNTIME_ROOT/codegeneration/test/c++/test2/$CXX_EXE_DIR/HelloWorld $INTEG_EXE_DIR/CdmwGen_test2

ln -s $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/$ADA_EXE_DIR/validation_server1 $INTEG_EXE_DIR/ada_server1
ln -s $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/$ADA_EXE_DIR/validation_server2 $INTEG_EXE_DIR/ada_server2
ln -s $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/$ADA_EXE_DIR/validation_client $INTEG_EXE_DIR/ada_client

# At this time no CPP integration test is written for the_validation_servers&client 
#ln -s $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/$CXX_EXE_DIR/server1 $INTEG_EXE_DIR/cpp_server1
#ln -s $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/$CXX_EXE_DIR/server2 $INTEG_EXE_DIR/cpp_server2
# But cpp client was used for ADA test: TODO: change test_ada.xml to have PFMNGT adaptation & Ada Client
#ln -s $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/$CXX_EXE_DIR/client $INTEG_EXE_DIR/cpp_client

ln -s $CDMW_RUNTIME_ROOT/ftmanager/c++/$CXX_EXE_DIR/cdmw_ft_manager $INTEG_EXE_DIR/cdmw_ft_manager
ln -s $INTEG_EXE_BUILD/c++/faulttolerance/test_server/$CXX_EXE_DIR/test_server $INTEG_EXE_DIR/test_server
ln -s $INTEG_EXE_BUILD/c++/faulttolerance/test_main/$CXX_EXE_DIR/cdmw_test_ft_init $INTEG_EXE_DIR/cdmw_test_ft_init


# Create links for the platformmngt
ln -s $INTEG_EXE_BUILD/c++/platformmngt/observer/$CXX_EXE_DIR/observer $INTEG_EXE_DIR/observer
ln -s $INTEG_EXE_BUILD/c++/platformmngt/process/$CXX_EXE_DIR/process $INTEG_EXE_DIR/process


ln -s $CDMW_RUNTIME_ROOT/xmlserver/java/script/cdmw_xml_server.sh $INTEG_EXE_DIR/cdmw_xml_server.sh
ln -s $CDMW_RUNTIME_ROOT/ccmpackagemanager/java/script/cdmw_package_manager.sh $INTEG_EXE_DIR/cdmw_package_manager.sh
ln -s $CDMW_RUNTIME_ROOT/ccmcomponentinstallation/java/script/cdmw_component_installation.sh $INTEG_EXE_DIR/cdmw_component_installation.sh
ln -s $CDMW_RUNTIME_ROOT/ccmserveractivator/java/script/cdmw_server_activator.sh $INTEG_EXE_DIR/cdmw_server_activator.sh
ln -s $CDMW_RUNTIME_ROOT/ccmassembly/java/script/cdmw_assembly_factory.sh $INTEG_EXE_DIR/cdmw_assembly_factory.sh
ln -s $CDMW_RUNTIME_ROOT/ccmdeploymenttool/java/script/cdmw_deployment_tool.sh $INTEG_EXE_DIR/cdmw_deployment_tool.sh

ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver1/$CXX_EXE_DIR/component_server1 $INTEG_EXE_DIR/component_server1 
ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/componentserver2/$CXX_EXE_DIR/component_server2 $INTEG_EXE_DIR/component_server2 
ln -s $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver1/data/component_server1.sh $INTEG_EXE_DIR/component_server1.sh 
ln -s $CDMW_RUNTIME_ROOT/integration/build/java/ccm/componentserver2/data/component_server2.sh $INTEG_EXE_DIR/component_server2.sh 

ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/GreekSchoolComponentServer/$CXX_EXE_DIR/greek_school_component_server $INTEG_EXE_DIR/greek_school_component_server 
ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/ObserverComponentServer/$CXX_EXE_DIR/observer_component_server $INTEG_EXE_DIR/observer_component_server 
ln -s $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/FrenchSchoolComponentServer/$CXX_EXE_DIR/french_school_component_server $INTEG_EXE_DIR/french_school_component_server
ln -s $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/GreekSchoolComponentServer/data/greek_school_component_server.sh $INTEG_EXE_DIR/greek_school_component_server.sh 
ln -s $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/ObserverComponentServer/data/observer_component_server.sh $INTEG_EXE_DIR/observer_component_server.sh 
ln -s $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/FrenchSchoolComponentServer/data/french_school_component_server.sh $INTEG_EXE_DIR/french_school_component_server.sh

cd $INTEG_DATA_DIR

##
# set mode of .in to generated file
#
# usage: set_mode_file file.in file.out
#
set_mode_file()
{
   # get mode of .in file 
	MODE=`ls -l $1 | cut -f1 -d' '`
	
	echo "Assign mode $MODE to $2"
   BIT=`echo $MODE | cut -b2`
	if [ $BIT = "r" ]; then
	   chmod u+r $2
	fi
	BIT=`echo $MODE | cut -b3`
   if [ $BIT = "w" ]; then
	   chmod u+w $2
	fi
	BIT=`echo $MODE | cut -b4`
   if [ $BIT = "x" ]; then
	   chmod u+x $2
	fi
	BIT=`echo $MODE | cut -b5`
	if [ $BIT = "r" ]; then
	   chmod g+r $2
	fi
	BIT=`echo $MODE | cut -b6`
   if [ $BIT = "w" ]; then
	   chmod g+w $2
	fi
	BIT=`echo $MODE | cut -b7`
   if [ $BIT = "x" ]; then
	   chmod g+x $2
	fi
	BIT=`echo $MODE | cut -b8`
	if [ $BIT = "r" ]; then
	   chmod o+r $2
	fi
	BIT=`echo $MODE | cut -b9`
   if [ $BIT = "w" ]; then
	   chmod o+w $2
	fi
	BIT=`echo $MODE | cut -b10`
   if [ $BIT = "x" ]; then
	   chmod o+x $2
	fi
}

#
# Generate file from .in
#
echo "Generating xml files from .in"
ext=in
sed_command="s/\.$ext\$//"
file_tmp=/tmp/generic_files.`$CDMW_RUNTIME_ROOT/bin/whoami.sh`.$$
HOSTNAME=`hostname`
for f in "*.xml.in"; do
   file_out=`echo $f | sed -e $sed_command`
   cat $f | sed \
   -e "s%@host_name@%$HOSTNAME%g" > $file_tmp

if [ ! -f "$file_out" ]
   then
       echo "Generating $file_out from $f ..."
       cp $file_tmp $file_out
       set_mode_file $f $file_out
   else  
       if [ -n "`diff $file_tmp $file_out`" ] 
       then
           # generated file is different from the already existing one 
           echo "Generating $file_out from $f ..."
           cp $file_tmp $file_out
           set_mode_file $f $file_out
       fi
   fi
   rm -f $file_tmp
done


# make BasicDemo package
echo "Create BasicDemo package"
if [ -f $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/data/BasicDemo.cad ]
then
   $CDMW_RUNTIME_ROOT/integration/build/c++/ccm/data/make_assembly_package.sh
fi
if [ -f $CDMW_RUNTIME_ROOT/integration/build/java/ccm/data/BasicDemo.cad ]
then
   $CDMW_RUNTIME_ROOT/integration/build/java/ccm/data/make_assembly_package.sh
fi
echo "Create Dinner package"
if [ -f $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/data/Dinner.cad ]
then
   $CDMW_RUNTIME_ROOT/integration/build/c++/ccm_philo/data/make_assembly_package.sh
fi
if [ -f $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/data/Dinner.cad ]
then
   $CDMW_RUNTIME_ROOT/integration/build/java/ccm_philo/data/make_assembly_package.sh
fi

echo "Installation finished"

