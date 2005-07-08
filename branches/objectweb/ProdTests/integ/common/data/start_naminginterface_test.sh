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

site_file=$CDMW_RUNTIME_ROOT/site.mk
JAVA_ORB="`cat $site_file | grep ORB_JAVA | cut -d= -f2`"
ORB_JAR_PATH="`cat $site_file | grep JAVA_JAR_PATH | cut -d= -f2`"

__CLASSPATH__=""

# add ORB jars
for jar in `ls $ORB_JAR_PATH/*.jar`
do
    __CLASSPATH__=$__CLASSPATH__$jar:
done


# add CDMW jars
__CLASSPATH__=$__CLASSPATH__\
$CDMW_RUNTIME_ROOT/common/java/prod/$JAVA_ORB/cdmwcommon.jar:\
$CDMW_RUNTIME_ROOT/ossupport/java/prod/$JAVA_ORB/cdmwossupport.jar:\
$CDMW_RUNTIME_ROOT/orbsupport/java/prod/$JAVA_ORB/cdmworbsupport.jar:\
$CDMW_RUNTIME_ROOT/idl/java/prod/$JAVA_ORB/cdmwidl.jar:\
$CDMW_RUNTIME_ROOT/naminginterface/java/prod/$JAVA_ORB/cdmwnaminginterface.jar:\
$CDMW_RUNTIME_ROOT/repositoryinterface/java/prod/$JAVA_ORB/cdmwrepositoryinterface.jar:\
$CDMW_RUNTIME_ROOT/platforminterface/java/prod/$JAVA_ORB/cdmwplatforminterface.jar:\
$CDMW_RUNTIME_ROOT/naminginterface/java/test/prod/$JAVA_ORB/cdmwnaminginterfacetest.jar

# set library path for java native library
ORB_CPP="`cat $site_file | grep ORB_CPP |cut -d= -f2`"
HOST_TYPE="`$CDMW_RUNTIME_ROOT/bin/config.guess`"
CXX_NAME="`cat $site_file | grep CXX_NAME |cut -d= -f2`"
CXX_VER="`cat $site_file | grep CXX_VER |cut -d= -f2`"
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$HOST_TYPE-$CXX_NAME-$CXX_VER
LIBPATH=$LIBPATH:$CDMW_RUNTIME_ROOT/lib/$ORB_CPP/$HOST_TYPE-$CXX_NAME-$CXX_VER


exec java  -Xbootclasspath/p:$__CLASSPATH__ cdmw.namingandrepository.test.MainTest $*

