#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

# @author Fabrizio Lorenna    <f.lorenna@vitrociset.it>

# @brief script to start the cdmw_testfacade_System

#echo "LD_LIBRARY_PATH= $LD_LIBRARY_PATH"

echo "Starting platform daemon ..."

./cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml start &

echo "Wait 10 seconds to be sure that platform daemon is up ..."
sleep 10

#insert the hostname in the file CdmwFTSystemMngtGroupCreator_conf.xml
sed -e 's/@host_name@/'$HOSTNAME'/' CdmwFTSystemMngtGroupCreator_conf.xml >ftemp
\mv -f ftemp CdmwFTSystemMngtGroupCreator_conf.xml

echo "Starting the ft manager ..."
cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml &

PID_FT=$! 

echo "Wait 15 seconds to be sure that ft manager is up ..."
sleep 15


echo "Starting supervision service ..."
cdmw_platform_supervision --creation-timeout=20000 --event-timeout=6000 --RequestDurationTime=20000000 --CdmwXMLFile=CdmwSystemMngtDatastoreConfig.xml --validate --FaultManagerRegistration=corbaloc::localhost:4555/fault_manager &

PID_SUP=$!
#echo "PID SUPERVISION SERVICE = $PID_SUP"

echo "Wait 15 seconds to be sure that supervision service is up ..."
sleep 15

#insert the hostname in the file HelloSystemDefinition.xml
sed -e 's/@host_name@/'$HOSTNAME'/' HelloSystemDefinition.xml >ftemp
\mv -f ftemp HelloSystemDefinition.xml

echo "Starting scenario definition ..."
./cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define HelloSystemDefinition.xml &
sleep 10
echo "Wait 10 seconds to be sure that is done scenario definition ..."

echo "Starting cdmw_testfacade_Application ... "

./cdmw_testfacade_Application

#kill supervision service
kill -9 $PID_SUP
#kill ftp manager
kill -9 $PID_FT

#kill platf. daemon
./cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml stop &


exit 0
