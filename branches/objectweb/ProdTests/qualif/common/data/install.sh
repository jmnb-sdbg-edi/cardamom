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

help()
{
    echo " Usage is :"
    echo "$0 --host1 xmlhostname1 --host2 xmlhostname2 [--host3 xmlhostname3] --targethost1 targethostname1 [--targethost2 targethostname2]  [--targethost3 targethostname3] [--data] [--cdmw-exe] [--orblib] [--tools] [--cdmwccm] [--codegen] [--xmlservice]"
    exit
}


if [ $# -lt 6 ];
then
   help;
fi

##
# Check CDMW_RUNTIME_ROOT
#
if [ "xx$CDMW_RUNTIME_ROOT" = "xx" ]; then
   print_error "Please define the environment variable CDMW_RUNTIME_ROOT."
   exit 1
fi


ONLY_DATA="false"
CDMW_EXE="false"
ORB_LIB="false"
CDMW_CCM="false"
CDMW_TOOLS="false"
CODE_GEN="false"
XML_SERVICE="false"

if [ "$1" = "--help" ];
then
   help
else
	if [ "$1" = "--host1" ]
	then
		xmlhostname1=$2
		echo "host1 = $2"
	fi
	if [ "$3" = "--host2" ]
	then
		xmlhostname2=$4
		echo "host2 = $4"
	fi
	if [ "$5" = "--host3" ]
	then
		xmlhostname3=$6
		echo "host3 = $6"
		shift 2
	fi
	if [ "$5" = "--targethost1" ]
	then
		targethostname1=$6
		echo "targethost1 = $6"
	fi
	if [ "$7" = "--targethost2" ]
	then
		targethostname2=$8
		echo "targethost2 = $8"
		shift 2
	fi
	if [ "$7" = "--targethost3" ]
	then
		targethostname3=$8
		echo "targethost2 = $8"
	fi
	if [ "$2" = "" ]||[ "$4" = "" ]||[ "$6" = "" ]
	then
		help
	fi

   REMOTE_HOST=$1   
fi

echo "xmlhostname1 = $xmlhostname1"
echo "xmlhostname2 = $xmlhostname2"
echo "xmlhostname3 = $xmlhostname3"


if [ "$7" = "--targethost3" ]
then
   shift 2
fi

if [ "$7" = "" ];
then
   echo "Installing all validation files"
   ONLY_DATA="true"
   CDMW_EXE="true"
   ORB_LIB="true"
   CDMW_CCM="true"
   CDMW_TOOLS="true"
   CODE_GEN="true"
   XML_SERVICE="true"
else
   shift 6
   while [ -n "$*" ]; do
      case $1 in
         --help)
            help;
            exit $ret_ok;
            ;;
         --data)
            echo "Installing data"
            ONLY_DATA="true"
            ;;
         --cdmw-exe)
            echo "Installing CDMW libraries and binaries"
            CDMW_EXE="true"
            ;;
         --orblib)
            echo "Installing ORB libraries and binaries"
            ORB_LIB="true"
            ;;
         --cdmwccm)
            echo "Installing CDMW CCM libraries and binaries"
            CDMW_CCM="true"
            echo "Installing XML Service (required by CCM)"
            XML_SERVICE="true"
            ;;
         --tools)
            echo "Installing tools needed by CDMW"
            CDMW_TOOLS="true"
            ;;
         --codegen)
            echo "Installing CDMW code generator"
            CODE_GEN="true"
            ;;
         --xmlservice)
            echo "Installing XML Service"
            XML_SERVICE="true"
            ;;
         *)
            echo $1 ": invalid option ...";
            help;
            exit 1;
            ;;
      esac
      shift
   done
fi






HOSTNAME1=$1
HOSTNAME2=$2
HOSTNAME3=$3
CONFIG_HOST=./config.host


echo "Creating $CONFIG_HOST file"

if [ -f $CONFIG_HOST ]; then
   rm $CONFIG_HOST;
fi
FTP_DIR=`cat  $CDMW_RUNTIME_ROOT/site.mk | grep FTP_DIR |cut -d= -f2`

echo "FTP_DIR=$FTP_DIR" >> $CONFIG_HOST

echo "HOSTNAME1=$xmlhostname1" >> $CONFIG_HOST
echo "HOSTNAME2=$xmlhostname2" >> $CONFIG_HOST
echo "HOSTNAME3=$xmlhostname3" >> $CONFIG_HOST

ROOT_DIR=/home2/cdmw/validation

echo "@CDMW_PLATFORM_DAEMON_PATH@=$ROOT_DIR/exe" >> $CONFIG_HOST
echo "@CDMW_PLATFORM_SCRIPTS_PATH@=$ROOT_DIR/exe" >> $CONFIG_HOST
echo "@CDMW_PLATFORM_DAEMON_SCRIPTS_PATH@=$ROOT_DIR/exe" >> $CONFIG_HOST

cd $CDMW_RUNTIME_ROOT

ORB_ADA="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_ADA |cut -d= -f2`"
ADA_LANG=FALSE
if [ "m$ORB_ADA" != "m" ]
then
   echo "ADA validation files are being installed!"
   ADA_LANG=TRUE
else
   echo "ADA validation files are NOT being installed!"
fi
WITH_CDMW_JAVA="`cat $CDMW_RUNTIME_ROOT/site.mk | grep WITH_CDMW_JAVA |cut -d= -f2`"
JAVA_LANG=FALSE
if [ "$WITH_CDMW_JAVA" != "no" ]
then
   echo "Java validation files are being installed!"
   JAVA_LANG=TRUE
else
   echo "Java validation files are NOT being installed!"
fi

cd $CDMW_RUNTIME_ROOT/validation/data

for file in `find . -name "*\.valid"`
do
   $CDMW_RUNTIME_ROOT/bin/generic_files.sh -f config.host -e valid -- $file;
done
for file in `find $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/data  -name "*\.valid"`
do
   $CDMW_RUNTIME_ROOT/bin/generic_files.sh -f config.host -e valid -- $file;
done

if [ "$ADA_LANG" = "TRUE" ]
then
   for file in `find $CDMW_RUNTIME_ROOT/validation/build/ada/eventclient/data  -name "*\.valid"`
   do
      $CDMW_RUNTIME_ROOT/bin/generic_files.sh -f config.host -e valid -- $file;
   done
fi

if [ "$JAVA_LANG" = "TRUE" ]
then
   for file in `find $CDMW_RUNTIME_ROOT/validation/build/java/eventclient/data  -name "*\.valid"`
   do
      $CDMW_RUNTIME_ROOT/bin/generic_files.sh -f config.host -e valid -- $file;
   done
fi






for REMOTE_HOST in $targethostname1 $targethostname2 $targethostname3
do

cd $CDMW_RUNTIME_ROOT/validation/data



echo "Installing in $REMOTE_HOST:$ROOT_DIR"

REMOTE_RUN_DIR=$REMOTE_HOST:$ROOT_DIR
REMOTE_EXE_DIR=$REMOTE_HOST:$ROOT_DIR/exe
REMOTE_LIB_DIR=$REMOTE_HOST:$ROOT_DIR/lib
REMOTE_JAR_DIR=$REMOTE_HOST:$ROOT_DIR/lib/jars
if [ "$ADA_LANG" = "TRUE" ]
then
   REMOTE_ORBADA_DIR=$REMOTE_HOST:$ROOT_DIR/exe/orbada
fi # ADA_LANG
REMOTE_IDL_DIR=$REMOTE_HOST:$ROOT_DIR/idl


echo "Creating the directories"
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/lib
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/lib/jars
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/lib/jars/OpenORB
if [ "$ADA_LANG" = "TRUE" ]
then
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/orbada
fi # ADA_LANG
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/OB
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/TAO
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/TAO/tao
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/Q-007
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/Q-018
rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/Q-027
if [ "$CDMW_CCM" = "true" ];
then
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/Q-028
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/idl/Q-029
fi

#rsh $REMOTE_HOST chmod a+rwx $ROOT_DIR
#rsh $REMOTE_HOST chmod a+rwx $ROOT_DIR/exe
rsh $REMOTE_HOST chmod a+rwx $ROOT_DIR $ROOT_DIR/exe

# We determine the site.mk variables
ORB_CPP="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_CPP |cut -d= -f2`"
ORB_JAVA="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORB_JAVA |cut -d= -f2`"

CXX_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_NAME |cut -d= -f2`"
CXX_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep CXX_VER |cut -d= -f2`"

if [ "$ADA_LANG" = "TRUE" ]
then
ADA_NAME="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ADA_NAME |cut -d= -f2`"
ADA_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ADA_VER |cut -d= -f2`"
fi # ADA_LANG

LIB_STD_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep STLPORT_VER |cut -d= -f2`"
LIB_OB_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_VER |cut -d= -f2`"
LIB_JTC_VER="`cat $CDMW_RUNTIME_ROOT/site.mk | grep JTC_VER |cut -d= -f2`"

ORBACUS_IDL_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_IDL_PATH |cut -d= -f2`"
ORBACUS_BIN_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_BIN_PATH |cut -d= -f2`"
ORBACUS_LIB_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_LIB_PATH |cut -d= -f2`"
ORBACUS_JAVA_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep ORBACUS_JAVA_JAR_PATH |cut -d= -f2`"
STLPORT_LIB_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep STLPORT_LIB_PATH |cut -d= -f2`"
XERCES_LIB_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep XERCES_LIB_PATH |cut -d= -f2`"
XERCES_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep XERCES_JAR_PATH |cut -d= -f2`"
XALAN_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep XALAN_JAR_PATH |cut -d= -f2`"
OPENORB_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep OPENORB_JAR_PATH |cut -d= -f2`"
OPENCCM_BIN_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep OPENCCM_BIN_PATH | cut -d= -f2`"
SAXON_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep SAXON_JAR_PATH | cut -d= -f2`"
FTP_JAR_PATH="`cat $CDMW_RUNTIME_ROOT/site.mk | grep FTP_JAR_PATH | cut -d= -f2`"

HOST_TYPE="`$CDMW_RUNTIME_ROOT/bin/config.guess`"
BIN_CXX=$HOST_TYPE-$CXX_NAME-$CXX_VER
BIN_ADA=$HOST_TYPE-$ADA_NAME-$ADA_VER

EXE_DIR="prod/$ORB_CPP/$BIN_CXX"
ADA_EXE_DIR="prod/$ORB_ADA/$BIN_ADA"
JAVA_EXE_DIR="prod/$ORB_JAVA"
ORB_CPP_NAME="`echo $ORB_CPP | cut -d'_' -f1`"

#LIB_PATH="/tools/exec/gcc-2.95.2/$HOST_TYPE/lib"
CXX_VER_WITH_PTS="`echo $CXX_VER | sed 's/-/./g'`"
LIB_PATH="/tools/exec/$CXX_NAME-$CXX_VER_WITH_PTS/$HOST_TYPE/lib"

MINIMAL_PATH=.:/usr/bin:/bin
echo "Creating set_env file"
echo "#! /bin/sh" > set_env$$.sh
JDK_JAR="`cat $CDMW_RUNTIME_ROOT/site.mk | grep JDK_JAR | cut -d= -f2`"
echo "LD_LIBRARY_PATH=$ROOT_DIR/lib" >> set_env$$.sh
echo "export LD_LIBRARY_PATH" >> set_env$$.sh
echo "PATH=$JDK_JAR/../../bin:$ROOT_DIR/exe:/usr/X11R6/bin/:$MINIMAL_PATH" >> set_env$$.sh
echo "export PATH" >> set_env$$.sh
if [ "$ADA_LANG" = "TRUE" ]
then
   echo "ORBADA_DIR=$ROOT_DIR/exe/orbada" >> set_env$$.sh
   echo "export ORBADA_DIR" >> set_env$$.sh
   echo "export ORBRIVER_DIR=$ORBRIVER_DIR" >> set_env$$.sh
fi # ADA_LANG
echo "OPENORB_JAR_PATH=$ROOT_DIR/lib/jars/OpenORB" >> set_env$$.sh
echo "export OPENORB_JAR_PATH" >> set_env$$.sh
if [ "$ORB_CPP_NAME" = "orbacus" ]
then
   echo "if [ -h nameserv ]; then" >> set_env$$.sh
   echo "   rm -f nameserv" >> set_env$$.sh
   echo "fi" >> set_env$$.sh
echo "ln -s exe/nameserv nameserv" >> set_env$$.sh
fi # ORB_CPP_NAME="orbacus"
if [ "$ORB_CPP_NAME" = "tao" ]
then
   echo "if [ -h Naming_Service ]; then" >> set_env$$.sh
   echo "   rm -f Naming_Service" >> set_env$$.sh
   echo "fi" >> set_env$$.sh
   echo "ln -s exe/Naming_Service Naming_Service" >> set_env$$.sh
fi # ORB_CPP_NAME="tao"

chmod a+rx set_env$$.sh

CDMW_HOST_EXE="cdmw_host*.sh"
echo "Creating set_root file"
echo "chown root:root $CDMW_HOST_EXE" > set_root$$.sh
echo "chmod 550 $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod a-w $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod o-r $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod o-x $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod g+r $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod g+x $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod u+r $CDMW_HOST_EXE" >> set_root$$.sh
#echo "chmod u+x $CDMW_HOST_EXE" >> set_root$$.sh

cd ../..

echo "Installing scripts"

rcp $CDMW_RUNTIME_ROOT/validation/data/set_env$$.sh $REMOTE_RUN_DIR/set_env.sh
rcp $CDMW_RUNTIME_ROOT/validation/data/set_root$$.sh $REMOTE_RUN_DIR/set_root.sh

rcp $CDMW_RUNTIME_ROOT/validation/data/platform_admin.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/start_observer.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/start_supervision.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/define_system.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/start_system.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/stop_system.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/start_daemon_verbosely.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/system_snapshot.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/run_validation_test.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/run_ccm_validation_test.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/Q-007*.sh $REMOTE_RUN_DIR
rcp $CDMW_RUNTIME_ROOT/validation/data/Q-027*.sh $REMOTE_RUN_DIR

rcp $CDMW_RUNTIME_ROOT/validation/data/install_inspected_data.sh $REMOTE_RUN_DIR

if [ "$ONLY_DATA" = "true" ];
then
   echo "Installing data"

   rcp $CDMW_RUNTIME_ROOT/validation/data/*.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/platformdaemon/c++/src/*.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/platformadmin/c++/src/*.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/namingandrepository/c++/src/*.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/namingandrepository/c++/src/*.xml $REMOTE_RUN_DIR
   if [ "$ADA_LANG" = "TRUE" ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/ada/traceclient/data/*.xml $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/ada/traceclient/data/*.dtd $REMOTE_RUN_DIR
   fi # ADA_LANG
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/data/*.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/data/*.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/data/ob_event.conf $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/data/*.cmd $REMOTE_RUN_DIR
   if [ -d $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/generated ]
   then
   	  rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/generated/TestEvent_Desc.dtd $REMOTE_RUN_DIR
   fi
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/idl/*.idl $REMOTE_IDL_DIR/Q-018
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/java/eventclient/data/tec_creation.cmd $REMOTE_RUN_DIR/tec_creation-java.cmd
      rcp $CDMW_RUNTIME_ROOT/validation/build/java/eventclient/data/ec_creation.cmd $REMOTE_RUN_DIR/ec_creation-java.cmd
      rcp $CDMW_RUNTIME_ROOT/validation/build/java/eventclient/data/EventChannelConf.xml $REMOTE_RUN_DIR/EventChannelConf-java.xml
      rcp $CDMW_RUNTIME_ROOT/validation/build/java/eventclient/idl/Echo.idl  $REMOTE_IDL_DIR/Q-018/Echo-java.idl
      rcp $CDMW_RUNTIME_ROOT/eventinterface/java/$JAVA_EXE_DIR/cdmweventinterface.jar $REMOTE_JAR_DIR
   fi # JAVA_LANG
   rcp $CDMW_RUNTIME_ROOT/export/idl/CosEventComm.idl $REMOTE_IDL_DIR/Q-018
   rcp $CDMW_RUNTIME_ROOT/export/idl/CdmwLifeCycle.idl $REMOTE_IDL_DIR
   rcp $CDMW_RUNTIME_ROOT/export/idl/CosLifeCycle.idl $REMOTE_IDL_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-007-data/*.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-007-data/*.idl $REMOTE_IDL_DIR/Q-007
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-027-data/*.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-027-data/*.idl $REMOTE_IDL_DIR/Q-027
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-031-data/*.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-033-data/*.xml $REMOTE_RUN_DIR
   if [ -d $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient1/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient1/generated/Hello.dtd $REMOTE_RUN_DIR
   fi

   if [ -d $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/generated/Client_Desc.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/data/Client.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/generated/Server1_Desc.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/data/Server1.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/generated/Server2_Desc.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/data/Server2.xml $REMOTE_RUN_DIR
   fi

   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/generated/Ada_Client_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/data/Client.xml $REMOTE_RUN_DIR/Ada_Client.xml
      fi
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/generated/Ada_Server1_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/data/Server1.xml $REMOTE_RUN_DIR/Ada_Server1.xml
      fi
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/generated/Ada_Server2_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/data/Server2.xml $REMOTE_RUN_DIR/Ada_Server2.xml
      fi
   fi # ADA_LANG

   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/generated/Java_Client_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/data/Client.xml $REMOTE_RUN_DIR/Java_Client.xml
      fi
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/generated/Java_Server1_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/data/Server1.xml $REMOTE_RUN_DIR/Java_Server1.xml
      fi
      if [ -d $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/generated ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/generated/Java_Server2_Desc.dtd $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/data/Server2.xml $REMOTE_RUN_DIR/Java_Server2.xml
      fi
   fi # JAVA_LANG

   if [ -d $CDMW_RUNTIME_ROOT/timeservice/c++/test/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/timeservice/c++/test/data/TimeServiceTest.xml $REMOTE_RUN_DIR 
      rcp $CDMW_RUNTIME_ROOT/timeservice/c++/test/generated/TimeServiceTest_Desc.dtd $REMOTE_RUN_DIR 
   fi

   # install inspected data for Q-011, Q-013, Q-014, Q-016, Q-019
   $CDMW_RUNTIME_ROOT/validation/data/install_inspected_data.sh --host=$REMOTE_HOST
fi

if [ "$XML_SERVICE" = "true" ];
then
   # install specific files for xml service validation test
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-030-data/cdmw_xml_server.sh $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-030-data/xml_service_test_client.sh $REMOTE_RUN_DIR
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/Q-030-data
   rsh $REMOTE_HOST chmod 777 $ROOT_DIR/Q-030-data
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-030-data/Q-030-file.xml $REMOTE_RUN_DIR/Q-030-data
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-030-data/Q-030-file.xml /home/ftp
fi

   
if [ "$CDMW_CCM" = "true" ];
then
   # install dtd for ccm
   rcp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/softpkg.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/corbacomponent.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/componentassembly.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmdeploymentutil/data/properties.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmpackagemanager/data/CcmPackageManagerSave.dtd $REMOTE_RUN_DIR

   # install specific files for ccm validation tests
   rcp $CDMW_RUNTIME_ROOT/export/idl/Components.idl $REMOTE_IDL_DIR
   rcp $CDMW_RUNTIME_ROOT/export/idl/CosPersistentState.idl $REMOTE_IDL_DIR
   rcp $CDMW_RUNTIME_ROOT/export/idl/SecurityLevel2.idl $REMOTE_IDL_DIR
   
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver1/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver1/generated/ComponentServer1.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver1/data/ComponentServer1.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver2/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver2/generated/ComponentServer2.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver2/data/ComponentServer2.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/clientcomponent/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/clientcomponent/generated/Basic_cif.idl $REMOTE_IDL_DIR/Q-028
   fi

   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/FrenchSchoolComponentServer/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/FrenchSchoolComponentServer/generated/FrenchSchoolComponentServer.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/FrenchSchoolComponentServer/data/FrenchSchoolComponentServer.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/GreekSchoolComponentServer/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/GreekSchoolComponentServer/generated/GreekSchoolComponentServer.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/GreekSchoolComponentServer/data/GreekSchoolComponentServer.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/ObserverComponentServer/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/ObserverComponentServer/generated/ObserverComponentServer.dtd $REMOTE_RUN_DIR
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/ObserverComponentServer/data/ObserverComponentServer.xml $REMOTE_RUN_DIR
   fi
   if [ -d $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/philosophercomponent/generated ]
   then
      rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/philosophercomponent/generated/Philo_cif.idl $REMOTE_IDL_DIR/Q-029
   fi
   rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/data/ccm_event.conf $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/data/EventChannelConf1.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/data/EventChannel_Desc.dtd $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/data/EventChannelManager.xml $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/data/EventChannelManager_Desc.dtd $REMOTE_RUN_DIR

   # install Q-028-*_commands.txt & Q-029-*_commands.txt
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-028-data/Q-028-1_commands.txt $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-028-data/Q-028-2_commands.txt $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-029-data/Q-029-1_commands.txt $REMOTE_RUN_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/data/Q-029-data/Q-029-2_commands.txt $REMOTE_RUN_DIR
fi # CDMW_CCM



if [ "$CDMW_EXE" = "true" ];
then
   echo "Installing CDMW executables"
   rcp $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_launch_process.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_kill_pid.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformlibrary/script/cdmw_platform_kill_all_pids.sh $REMOTE_EXE_DIR

   mkdir -p $CDMW_RUNTIME_ROOT/validation/data/temp;
   cp $CDMW_RUNTIME_ROOT/platformdaemon/c++/script/cdmw_platform_daemon.sh.in $CDMW_RUNTIME_ROOT/validation/data/temp/cdmw_platform_daemon.sh.valid

   for file in `find $CDMW_RUNTIME_ROOT/validation/data/temp  -name "*\.valid"`
   do
	$CDMW_RUNTIME_ROOT/bin/generic_files.sh -f $CDMW_RUNTIME_ROOT/validation/data/config.host -e valid -- $file;
   done
   
   rcp $CDMW_RUNTIME_ROOT/validation/data/temp/cdmw_platform_daemon.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformdaemon/c++/script/cdmw_platform_daemon_monitoring.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformdaemon/c++/$EXE_DIR/cdmw_platform_daemon $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformagent/c++/$EXE_DIR/cdmw_platform_agent $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformsystem/c++/$EXE_DIR/cdmw_platform_supervision $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformadmin/c++/$EXE_DIR/cdmw_platform_admin $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/platformsupervisionobserver/c++/$EXE_DIR/cdmw_platform_supervision_observer $REMOTE_EXE_DIR
   if [ -f $CDMW_RUNTIME_ROOT/platforminterface/c++/test/testmanagedprocess/$EXE_DIR/cdmw_test_managed_process ]
   then
      rcp $CDMW_RUNTIME_ROOT/platforminterface/c++/test/testmanagedprocess/$EXE_DIR/cdmw_test_managed_process $REMOTE_EXE_DIR
   fi
   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/platforminterface/ada/test/test_managed_process/$ADA_EXE_DIR/cdmw_test_managed_process ]
      then
         rcp $CDMW_RUNTIME_ROOT/platforminterface/ada/test/test_managed_process/$ADA_EXE_DIR/cdmw_test_managed_process $REMOTE_EXE_DIR/cdmw_ada_test_managed_process
      fi
   fi # ADA_LANG
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/platforminterface/java/test/testmanagedprocess/$JAVA_EXE_DIR/cdmwplatforminterfacetest.jar ]
      then
         rcp $CDMW_RUNTIME_ROOT/platforminterface/java/test/testmanagedprocess/$JAVA_EXE_DIR/cdmwplatforminterfacetest.jar $REMOTE_JAR_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/cdmw_platforminterface_test.sh $REMOTE_RUN_DIR
      fi
   fi # JAVA_LANG

   rcp $CDMW_RUNTIME_ROOT/bin/cdmw_host*.sh $REMOTE_RUN_DIR

   rcp $CDMW_RUNTIME_ROOT/tracecollector/c++/$EXE_DIR/cdmw_trace_collector $REMOTE_EXE_DIR

   if [ -f $CDMW_RUNTIME_ROOT/performancelibrary/c++/test/test2/$EXE_DIR/Performance ]
   then
      rcp $CDMW_RUNTIME_ROOT/performancelibrary/c++/test/test2/$EXE_DIR/Performance $REMOTE_EXE_DIR/perf_tracer
   fi
   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/validation/build/ada/performance/$ADA_EXE_DIR/test_performance \
         -a -f $CDMW_RUNTIME_ROOT/validation/build/ada/traceclient/$ADA_EXE_DIR/test_trace ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/performance/$ADA_EXE_DIR/test_performance $REMOTE_EXE_DIR/ada_perf_tracer
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/traceclient/$ADA_EXE_DIR/test_trace $REMOTE_EXE_DIR/cdmw_ada_test_trace
      fi
   fi # ADA_LANG
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/performancelibrary/java/test/$JAVA_EXE_DIR/cdmwperformancelibrarytest.jar ]
      then
         rcp $CDMW_RUNTIME_ROOT/performancelibrary/java/test/$JAVA_EXE_DIR/cdmwperformancelibrarytest.jar $REMOTE_JAR_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/cdmw_performance_test.sh $REMOTE_RUN_DIR
      fi
   fi # JAVA_LANG

   rcp $CDMW_RUNTIME_ROOT/namingandrepository/c++/$EXE_DIR/cdmw_naming_and_repository $REMOTE_EXE_DIR
   if [ -f $CDMW_RUNTIME_ROOT/namingandrepository/c++/test/test1/$EXE_DIR/cdmw_test_naming_and_repository ]
   then
      rcp $CDMW_RUNTIME_ROOT/namingandrepository/c++/test/test1/$EXE_DIR/cdmw_test_naming_and_repository $REMOTE_EXE_DIR
   fi

   if [ -f $CDMW_RUNTIME_ROOT/integration/build/c++/naminginterface/$EXE_DIR/testNI ]
   then
      rcp $CDMW_RUNTIME_ROOT/integration/build/c++/naminginterface/$EXE_DIR/testNI $REMOTE_EXE_DIR
   fi
   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/naminginterface/ada/test/$ADA_EXE_DIR/testni ]
      then
         rcp $CDMW_RUNTIME_ROOT/naminginterface/ada/test/$ADA_EXE_DIR/testni $REMOTE_EXE_DIR/ada_testNI
      fi
   fi # ADA_LANG
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/naminginterface/java/test/$JAVA_EXE_DIR/cdmwnaminginterfacetest.jar ]
      then 
         rcp $CDMW_RUNTIME_ROOT/naminginterface/java/test/$JAVA_EXE_DIR/cdmwnaminginterfacetest.jar $REMOTE_JAR_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/cdmw_namingandrepository_test.sh $REMOTE_RUN_DIR
      fi
   fi # JAVA_LANG

   rcp $CDMW_RUNTIME_ROOT/eventadmin/c++/$EXE_DIR/cdmw_event_admin $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/eventchannelmanager/c++/$EXE_DIR/cdmw_event_channel_manager $REMOTE_EXE_DIR
   if [ -f $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/$EXE_DIR/testEVT ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/eventclient/$EXE_DIR/testEVT $REMOTE_EXE_DIR
   fi
   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/validation/build/ada/eventclient/$ADA_EXE_DIR/test_event ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/eventclient/$ADA_EXE_DIR/test_event $REMOTE_EXE_DIR/ada_testEVT
      fi
   fi # ADA_LANG
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/eventinterface/java/test/test_interface/$JAVA_EXE_DIR/cdmweventinterfacetest.jar ]
      then
         rcp $CDMW_RUNTIME_ROOT/eventinterface/java/test/test_interface/$JAVA_EXE_DIR/cdmweventinterfacetest.jar $REMOTE_JAR_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/cdmw_event_test.sh $REMOTE_RUN_DIR
      fi
   fi # JAVA_LANG

   if [ -f $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/$EXE_DIR/client \
      -a -f $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/$EXE_DIR/server1 \
      -a -f $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/$EXE_DIR/server2 ]
   then
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleclient/$EXE_DIR/client $REMOTE_EXE_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver1/$EXE_DIR/server1 $REMOTE_EXE_DIR
      rcp $CDMW_RUNTIME_ROOT/validation/build/c++/lifecycleserver2/$EXE_DIR/server2 $REMOTE_EXE_DIR
   fi

   if [ "$ADA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/$ADA_EXE_DIR/validation_client \
         -a -f $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/$ADA_EXE_DIR/validation_server1 \
         -a -f $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/$ADA_EXE_DIR/validation_server2 ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleclient/$ADA_EXE_DIR/validation_client $REMOTE_EXE_DIR/ada_client
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver1/$ADA_EXE_DIR/validation_server1 $REMOTE_EXE_DIR/ada_server1
         rcp $CDMW_RUNTIME_ROOT/validation/build/ada/lifecycleserver2/$ADA_EXE_DIR/validation_server2 $REMOTE_EXE_DIR/ada_server2
      fi
   fi # ADA_LANG

   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/$JAVA_EXE_DIR/cdmwvalidation.jar \
   	     -a -f $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/$JAVA_EXE_DIR/cdmwvalidation.jar \
   	     -a -f $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/$JAVA_EXE_DIR/cdmwvalidation.jar ]
      then
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleclient/$JAVA_EXE_DIR/cdmwvalidation.jar $REMOTE_JAR_DIR/cdmwclient.jar
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/client.sh $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver1/$JAVA_EXE_DIR/cdmwvalidation.jar $REMOTE_JAR_DIR/cdmwserver1.jar
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/server1.sh $REMOTE_RUN_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/build/java/lifecycleserver2/$JAVA_EXE_DIR/cdmwvalidation.jar $REMOTE_JAR_DIR/cdmwserver2.jar
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/server2.sh $REMOTE_RUN_DIR
         
      fi
   fi # JAVA_LANG

   if [ -f $CDMW_RUNTIME_ROOT/timeservice/c++/test/$EXE_DIR/timeservicetest ]
   then
      rcp $CDMW_RUNTIME_ROOT/timeservice/c++/test/$EXE_DIR/timeservicetest $REMOTE_EXE_DIR
   fi
   if [ "$JAVA_LANG" = "TRUE" ]
   then
      if [ -f $CDMW_RUNTIME_ROOT/timeservice/java/test/$JAVA_EXE_DIR/cdmwtimeservicetest.jar ]
      then
         rcp $CDMW_RUNTIME_ROOT/timeservice/java/test/$JAVA_EXE_DIR/cdmwtimeservicetest.jar $REMOTE_JAR_DIR
         rcp $CDMW_RUNTIME_ROOT/validation/data/java_script/cdmw_timeservice_test.sh $REMOTE_RUN_DIR
      fi
   fi # JAVA_LANG

   rcp $CDMW_RUNTIME_ROOT/ftmanager/c++/$EXE_DIR/cdmw_ft_manager $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient1/$EXE_DIR/cdmw_faulttolerance_client1 $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient1/script/launchFaultToleranceClient1.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient2/$EXE_DIR/cdmw_faulttolerance_client2 $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceclient2/script/launchFaultToleranceClient2.sh $REMOTE_EXE_DIR
   rcp $CDMW_RUNTIME_ROOT/validation/build/c++/faulttolerance/faulttoleranceserver/$EXE_DIR/cdmw_faulttolerance_server $REMOTE_EXE_DIR

fi # CDMW_EXE


if [ "$ORB_LIB" = "true" ];
then
   if [ "$ORB_CPP_NAME" = "orbacus" ];
   then
      echo "Installing ORBACUS files"
      rcp $ORBACUS_BIN_PATH/nameserv $REMOTE_EXE_DIR
      rcp $ORBACUS_BIN_PATH/eventserv $REMOTE_EXE_DIR
      rcp $ORBACUS_BIN_PATH/irserv $REMOTE_EXE_DIR
      rcp $ORBACUS_BIN_PATH/idlcpp $REMOTE_EXE_DIR
      rcp $ORBACUS_LIB_PATH/libOB.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libJTC.so.$LIB_JTC_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libCosNaming.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libOBNaming.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libOBNamingService.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libCosProperty.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libCosEvent.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libOBEventService.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libOBEvent.so.$LIB_OB_VER $REMOTE_LIB_DIR
      rcp $ORBACUS_LIB_PATH/libIDL.so.$LIB_OB_VER $REMOTE_LIB_DIR
   fi # ORB_CPP_NAME="orbacus"

   echo "Installing Java ORBACUS jar files"
   rcp $ORBACUS_JAVA_JAR_PATH/*.jar $REMOTE_JAR_DIR 
   rcp $FTP_JAR_PATH/*.jar $REMOTE_JAR_DIR 


   if [ "$ORB_CPP_NAME" = "tao" ];
   then
      echo "Installing ACE files"
      rcp $ACE_ROOT/ace/libACE.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_PortableServer.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_CosNaming.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_CosEvent.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_Strategies.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_CosProperty.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_Messaging.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_RTPortableServer.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_RTCORBA.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_Svc_Utils.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_IORTable.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_IFRService.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_IFR_Client.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_TypeCodeFactory.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_FTORB.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_FaultTolerance.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_CosNotification.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_IORManip.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_ETCL.so* $REMOTE_LIB_DIR
      rcp $ACE_ROOT/ace/libTAO_DynamicAny.so* $REMOTE_LIB_DIR

      rcp $TAO_ROOT/orbsvcs/Naming_Service/Naming_Service $REMOTE_EXE_DIR
      rcp $TAO_ROOT/orbsvcs/IFR_Service/IFR_Service $REMOTE_EXE_DIR
   fi # ORB_CPP_NAME="tao"

   if [ "$ADA_LANG" = "TRUE" ]
   then
      echo "Installing ORBADA files"
      rcp -r $ORBRIVER_DIR/etc $REMOTE_ORBADA_DIR
      rcp -r $ORBRIVER_DIR/bin $REMOTE_ORBADA_DIR
   fi # ADA_LANG

fi # ORB_LIB

if [ "$CDMW_TOOLS" = "true" ];
then
   echo "Installing STL files"
   rcp $LIB_PATH/*.so.* $REMOTE_LIB_DIR
   rcp $STLPORT_LIB_PATH/libstlport_gcc.so $REMOTE_LIB_DIR
   rcp $STLPORT_LIB_PATH/libstlport_gcc_debug.so $REMOTE_LIB_DIR
   rcp $STLPORT_LIB_PATH/libstlport_gcc_stldebug.so $REMOTE_LIB_DIR

   echo "Installing XERCES files"
   rcp $XERCES_LIB_PATH/libxerces*.so $REMOTE_LIB_DIR 
   rcp $XERCES_JAR_PATH/*.jar $REMOTE_JAR_DIR 
    
   echo "Installing XALAN files"
   rcp $XALAN_JAR_PATH/*.jar $REMOTE_JAR_DIR

   echo "Installing SAXON files"
   rcp $SAXON_JAR_PATH/saxon.jar $REMOTE_JAR_DIR

   echo "Installing OpenORB files"
   rcp $OPENORB_JAR_PATH/*.jar $REMOTE_JAR_DIR/OpenORB

   echo "Installing TimeService files"
   rcp $CDMW_RUNTIME_ROOT/timeservice/data/start_time_service.sh $REMOTE_RUN_DIR 
fi # CDMW_TOOLS

if [ "$XML_SERVICE" = "true" ];
then
   echo "Installing XML Service files"
   rcp -r $CDMW_RUNTIME_ROOT/xmllibrary/java/$JAVA_EXE_DIR/cdmwxmllibrary.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/xmlserver/java/$JAVA_EXE_DIR/cdmwxmlserver.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/xmlserverlib/java/$JAVA_EXE_DIR/cdmwxmlserverlib.jar $REMOTE_JAR_DIR
   if [ -f $CDMW_RUNTIME_ROOT/validation/build/java/xmlclient/$JAVA_EXE_DIR/cdmwvalidation.jar ]
   then
	rcp $CDMW_RUNTIME_ROOT/validation/build/java/xmlclient/$JAVA_EXE_DIR/cdmwvalidation.jar $REMOTE_JAR_DIR/cdmwxmlclient.jar
   fi	
   if [ -f $CDMW_RUNTIME_ROOT/xmllibrary/java/test/test_library/$JAVA_EXE_DIR/cdmwxmllibrarytest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/xmllibrary/java/test/test_library/$JAVA_EXE_DIR/cdmwxmllibrarytest.jar $REMOTE_JAR_DIR
   fi
fi

if [ "$CDMW_CCM" = "true" ];
then
   echo "Installing Cdmw dynamic library"
   rcp -r $CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$BIN_CXX/*.so $REMOTE_LIB_DIR

   echo "Installing CdmwCCM files"
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_xml_server.sh $REMOTE_RUN_DIR
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_package_manager.sh $REMOTE_RUN_DIR
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_component_installation.sh $REMOTE_RUN_DIR
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_server_activator.sh $REMOTE_RUN_DIR
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_assembly_factory.sh $REMOTE_RUN_DIR
   rcp -r $CDMW_RUNTIME_ROOT/validation/data/ccm_script/cdmw_deployment_tool.sh $REMOTE_RUN_DIR

   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver1/$EXE_DIR/component_server1 ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver1/$EXE_DIR/component_server1 $REMOTE_EXE_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/componentserver1/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/componentserver1/$JAVA_EXE_DIR/cdmwccmciftest.jar $REMOTE_JAR_DIR/cdmwcomponentserver1.jar
      rcp -r $CDMW_RUNTIME_ROOT/validation/data/java_script/component_server1.sh $REMOTE_RUN_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver2/$EXE_DIR/component_server2 ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/componentserver2/$EXE_DIR/component_server2 $REMOTE_EXE_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/componentserver2/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/componentserver2/$JAVA_EXE_DIR/cdmwccmciftest.jar $REMOTE_JAR_DIR/cdmwcomponentserver2.jar
      rcp -r $CDMW_RUNTIME_ROOT/validation/data/java_script/component_server2.sh $REMOTE_RUN_DIR
   fi

   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/FrenchSchoolComponentServer/$EXE_DIR/french_school_component_server ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/FrenchSchoolComponentServer/$EXE_DIR/french_school_component_server $REMOTE_EXE_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/FrenchSchoolComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/FrenchSchoolComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar $REMOTE_JAR_DIR/cdmwfrenchschoolcomponentserver.jar
      rcp -r $CDMW_RUNTIME_ROOT/validation/data/java_script/french_school_component_server.sh $REMOTE_RUN_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/GreekSchoolComponentServer/$EXE_DIR/greek_school_component_server ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/GreekSchoolComponentServer/$EXE_DIR/greek_school_component_server $REMOTE_EXE_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/GreekSchoolComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/GreekSchoolComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar $REMOTE_JAR_DIR/cdmwgreekschoolcomponentserver.jar
      rcp -r $CDMW_RUNTIME_ROOT/validation/data/java_script/greek_school_component_server.sh $REMOTE_RUN_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/ObserverComponentServer/$EXE_DIR/observer_component_server ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/ObserverComponentServer/$EXE_DIR/observer_component_server $REMOTE_EXE_DIR
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/ObserverComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/ObserverComponentServer/$JAVA_EXE_DIR/cdmwccmciftest.jar $REMOTE_JAR_DIR/cdmwobservercomponentserver.jar
      rcp -r $CDMW_RUNTIME_ROOT/validation/data/java_script/observer_component_server.sh $REMOTE_RUN_DIR
   fi

   rcp -r $CDMW_RUNTIME_ROOT/export/idl/Components.idl $REMOTE_IDL_DIR/Q-028
   rcp -r $CDMW_RUNTIME_ROOT/export/idl/SecurityLevel2.idl $REMOTE_IDL_DIR/Q-028
   rcp -r $CDMW_RUNTIME_ROOT/export/idl/CosPersistentState.idl $REMOTE_IDL_DIR/Q-028
   rcp -r $CDMW_RUNTIME_ROOT/export/idl/Components.idl $REMOTE_IDL_DIR/Q-029
   rcp -r $CDMW_RUNTIME_ROOT/export/idl/SecurityLevel2.idl $REMOTE_IDL_DIR/Q-029
   rcp -r $CDMW_RUNTIME_ROOT/export/idl/CosPersistentState.idl $REMOTE_IDL_DIR/Q-029

   if [ "$ORB_CPP_NAME" = "orbacus" ]
   then
      # copy Obacus IDL files needed by Components.idl
      rcp -r $ORBACUS_IDL_PATH/*/*.idl $REMOTE_IDL_DIR/OB
   fi

   if [ "$ORB_CPP_NAME" = "tao" ]
   then
      # copy TAO IDL files needed by Components.idl
      rcp -r $TAO_ROOT/orbsvcs/orbsvcs/*.idl $REMOTE_IDL_DIR/TAO
      rcp -r $TAO_ROOT/tao/IFR_Client/IFR_Base.pidl $REMOTE_IDL_DIR/TAO
      rcp -r $TAO_ROOT/tao/*.idl $REMOTE_IDL_DIR/TAO/tao
      rcp -r $TAO_ROOT/tao/*.pidl $REMOTE_IDL_DIR/TAO/tao
   fi

   # copy Jar files needed by CCM java tools and XML server
   rcp -r $CDMW_RUNTIME_ROOT/ccmassembly/java/$JAVA_EXE_DIR/cdmwccmassembly.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmcif/java/$JAVA_EXE_DIR/cdmwccmcif.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmcomponentinstallation/java/$JAVA_EXE_DIR/cdmwccmcomponentinstallation.jar  $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmconfigurator/java/$JAVA_EXE_DIR/cdmwccmconfigurator.jar  $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmdeploymentutil/java/$JAVA_EXE_DIR/cdmwccmdeploymentutil.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmdeploymenttool/java/$JAVA_EXE_DIR/cdmwccmdeploymenttool.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmpackagemanager/java/$JAVA_EXE_DIR/cdmwccmpackagemanager.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmcontainer/java/$JAVA_EXE_DIR/cdmwccmcontainer.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmcomponentserver/java/$JAVA_EXE_DIR/cdmwccmcomponentserver.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/codegeneration/java/$JAVA_EXE_DIR/cdmwcodegeneration.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/common/java/$JAVA_EXE_DIR/cdmwcommon.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/testutils/java/$JAVA_EXE_DIR/cdmwtestutils.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/idl/java/$JAVA_EXE_DIR/cdmwidl.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/lifecycle/java/$JAVA_EXE_DIR/cdmwlifecycle.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/naminginterface/java/$JAVA_EXE_DIR/cdmwnaminginterface.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/orbsupport/java/$JAVA_EXE_DIR/cdmworbsupport.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ossupport/java/$JAVA_EXE_DIR/cdmwossupport.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/cdmwinit/java/$JAVA_EXE_DIR/cdmwcdmwinit.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/performancelibrary/java/$JAVA_EXE_DIR/cdmwperformancelibrary.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/platforminterface/java/$JAVA_EXE_DIR/cdmwplatforminterface.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/repositoryinterface/java/$JAVA_EXE_DIR/cdmwrepositoryinterface.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/tracelibrary/java/$JAVA_EXE_DIR/cdmwtracelibrary.jar $REMOTE_JAR_DIR
   rcp -r $CDMW_RUNTIME_ROOT/ccmserveractivator/java/$JAVA_EXE_DIR/cdmwccmserveractivator.jar $REMOTE_JAR_DIR

   # make BasicDemo and Dinner packages
   echo "Create BasicDemo package"
   export CDMW_RUNTIME_ROOT
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/clientcomponent/$EXE_DIR/libClientBasicDemo.so \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration/servercomponent/$EXE_DIR/libServerBasicDemo.so ]
   then
      $CDMW_RUNTIME_ROOT/validation/data/make_basic_demo_package.sh
      rcp -r BasicDemo.aar $REMOTE_RUN_DIR
      rm -f BasicDemo.aar
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/clientcomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration/servercomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      $CDMW_RUNTIME_ROOT/validation/data/make_java_basic_demo_package.sh
      rcp -r JavaBasicDemo.aar $REMOTE_RUN_DIR
      rm -f JavaBasicDemo.aar
   fi
   echo "Create Dinner package"
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/observercomponent/$EXE_DIR/libObserver.so \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/philosophercomponent/$EXE_DIR/libPhilosopher.so \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/registratorcomponent/$EXE_DIR/libRegistrator.so \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/c++/test/integration_philo/forkmanagercomponent/$EXE_DIR/libForkManager.so ]
   then
      $CDMW_RUNTIME_ROOT/validation/data/make_dinner_package.sh
      rcp -r Dinner.aar $REMOTE_RUN_DIR
      rm -f Dinner.aar
   fi
   if [ -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/observercomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/philosophercomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/registratorcomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar \
      -a -f $CDMW_RUNTIME_ROOT/ccmcif/java/test/integration_philo/forkmanagercomponent/$JAVA_EXE_DIR/cdmwccmciftest.jar ]
   then
      $CDMW_RUNTIME_ROOT/validation/data/make_java_dinner_package.sh
      rcp -r JavaDinner.aar $REMOTE_RUN_DIR
      rm -f JavaDinner.aar
   fi
fi # CDMW_CCM

if [ "$CODE_GEN" = "true" ];
then
   TMPDIR="/tmp/${USER}_$$"
   mkdir $TMPDIR

   echo "Installing OpenCCM files"
   OPENCCM_DIRNAME=`dirname $OPENCCM_BIN_PATH | xargs basename`
   $CDMW_RUNTIME_ROOT/validation/data/install_customized_openccm.sh --host=$REMOTE_HOST

   echo "Installing the CDMW Code Generator"
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/script
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/dtd
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/templates/c++
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/templates/ada
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/templates/java
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/templates/common
   rsh $REMOTE_HOST mkdir -p $ROOT_DIR/exe/codegeneration/templates/openccm

   rcp $CDMW_RUNTIME_ROOT/codegeneration/dtd/*.dtd $REMOTE_EXE_DIR/codegeneration/dtd
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/common/*.xslt $REMOTE_EXE_DIR/codegeneration/templates/common
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/c++/*.xslt $REMOTE_EXE_DIR/codegeneration/templates/c++
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/ada/*.xslt $REMOTE_EXE_DIR/codegeneration/templates/ada
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/java/*.xslt $REMOTE_EXE_DIR/codegeneration/templates/java
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/openccm/*.template $REMOTE_EXE_DIR/codegeneration/templates/openccm
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/openccm/jidlscript.properties $REMOTE_EXE_DIR/codegeneration/templates/openccm
   rcp $CDMW_RUNTIME_ROOT/codegeneration/templates/openccm/cdmwidl-mapping.properties $REMOTE_EXE_DIR/codegeneration/templates/openccm
   rcp $CDMW_RUNTIME_ROOT/codegeneration/java/$JAVA_EXE_DIR/XMLValidator.class $REMOTE_EXE_DIR/codegeneration/script


   # in case of TAO An additional option is required for running ir
   if [ "`echo $ORB_CPP | sed -e 's/tao_.*/tao_/'`" = "tao_" ]
   then
	ADDITIONAL_IR_OPTIONS="-DACE_TAO"
   else
	ADDITIONAL_IR_OPTIONS=""
   fi
   for f in `find $CDMW_RUNTIME_ROOT/codegeneration/script/ -name "*.xslt.in"`; do \
      cat $f | sed s%@runtime_root@%$ROOT_DIR/exe%g > $TMPDIR/`basename $f | sed 's/.in//'`
   done
   rcp $TMPDIR/*.xslt $REMOTE_EXE_DIR/codegeneration/script
   cat > $TMPDIR/sed_patterns << _EOF_
   s%@runtime_root@%$ROOT_DIR/exe%g
   s%openccm_bindir=@CDMW_BIN_PATH@%openccm_bindir=$ROOT_DIR/exe/$OPENCCM_DIRNAME/bin%g
   s%@CDMW_BIN_PATH@%$ROOT_DIR/exe/codegeneration/script%g
   s%@CDMW_DATA_PATH@%$ROOT_DIR/exe/codegeneration/script%g
   s%@SAXON_JAR_PATH@%$ROOT_DIR/lib/jars%g
   s%@OPENCCM_BIN_PATH@%$ROOT_DIR/exe/$OPENCCM_DIRNAME/bin%g
   s%@CDMW_IDL_PATH@%$ROOT_DIR/idl%g
   s%@ADDITIONAL_IR_OPTIONS@%$ADDITIONAL_IR_OPTIONS%g
   s%@XERCES_JAR_PATH@%$ROOT_DIR/lib/jars%g
   s%xmi_dtd\=\$share_dir%xmi_dtd\=$ROOT_DIR/exe/codegeneration/dtd%g
_EOF_
   cat $CDMW_RUNTIME_ROOT/codegeneration/script/cdmw_export.sh.in | \
      sed -f $TMPDIR/sed_patterns > $TMPDIR/cdmw_export.sh
   cat $CDMW_RUNTIME_ROOT/codegeneration/script/cdmw_code_generator.sh.in | \
      sed -f $TMPDIR/sed_patterns > $TMPDIR/cdmw_code_generator.sh
   rcp $TMPDIR/cdmw*.sh $REMOTE_EXE_DIR/codegeneration/script
   rm -Rf $TMPDIR

fi # CODE_GEN

done # end for

cd $CDMW_RUNTIME_ROOT/validation/data
rm set_env$$.sh
rm set_root$$.sh

echo "Installation finished"

