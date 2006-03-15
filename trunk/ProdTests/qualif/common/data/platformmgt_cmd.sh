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

Q-022 test
==========
define_system.sh Q-022-1-1.xml
start_system.sh
platform_admin.sh --sys-add-application Q-022-2-1.xml
platform_admin.sh --app-initialise APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --app-add-process APPL4 Q-022-3-1.xml
platform_admin.sh --proc-initialise APPL4 P42 @hostname1val@
platform_admin.sh --proc-run APPL4 P42 @hostname1val@
platform_admin.sh --proc-stop APPL3 P32  @hostname2val@
platform_admin.sh --proc-stop APPL4 P41 @hostname1val@ EMERGENCY  
platform_admin.sh --app-stop APPL4
platform_admin.sh --app-stop APPL3 EMERGENCY
stop_system.sh
platform_admin.sh --host-shutdown=anakin
platform_admin.sh --host-reboot=anakin


Q-023 test
==========
define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13 @hostname2val@
platform_admin.sh --app-resume APPL1
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh Q-023-2-1.xml
start_system.sh
platform_admin.sh --proc-set-info APPL2 P22 @hostname1val@ EXEC=cdmw_test_managed_process PORT=21882 ARGS=--nb-steps=1
platform_admin.sh --proc-initialise APPL2 P22 @hostname1val@
platform_admin.sh --app-resume APPL2
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --sys-add-application Q-023-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41  @hostname1val@ Q-023-4.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ Q-023-5.xml
platform_admin.sh --proc-initialise APPL4 P41 @hostname1val@ LAST_STATE 10000 nothing
platform_admin.sh --app-resume APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ Q-023-6.xml
platform_admin.sh --proc-initialise APPL4 P41 @hostname1val@ LAST_STATE 10000 nothing
platform_admin.sh --proc-run APPL4 P41 @hostname1val@
platform_admin.sh --app-acknowledge APPL4
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13 @hostname2val@
platform_admin.sh --proc-next-step APPL1 P13 @hostname2val@
platform_admin.sh --proc-run APPL1 P13 @hostname2val@
platform_admin.sh --app-acknowledge APPL1
platform_admin.sh --sys-acknowledge


ps aux | grep "unmanaged"
platform_admin.sh --proc-initialise APPL3 P31 @hostname2val@
platform_admin.sh --app-acknowledge APPL3
platform_admin.sh --sys-acknowledge

ps aux | grep "unmanaged"
platform_admin.sh --proc-initialise APPL2 P21 @hostname2val@





platform_admin.sh --proc-initialise APPL1 P13  @hostname2val@
platform_admin.sh --proc-initialise APPL1 P14 @hostname2val@
platform_admin.sh --proc-next-step APPL1 P13 @hostname2val@
platform_admin.sh --proc-next-step APPL1 P14 @hostname2val@
platform_admin.sh --proc-run APPL1 P13 @hostname2val@
platform_admin.sh --proc-run APPL1 P14 @hostname2val@
platform_admin.sh --app-acknowledge APPL1

platform_admin.sh --app-stop APPL3
platform_admin.sh --app-initialise APPL3
platform_admin.sh --app-run APPL3
platform_admin.sh --app-acknowledge APPL3
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh Q-023-1-1.xml
platform_admin.sh --sys-add-application Q-023-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ Q-023-7.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --app-run APPL4
platform_admin.sh --app-stop APPL4

Q-048 test
==========

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml | tee CdmwValidationTests_Sys1.output

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_Sys2_conf.xml | tee CdmwValidationTests_Sys2.output

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_Sys2_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

start_supervisionSys1.sh 

start_supervisionSys2.sh 

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-define Q-048-1-1.xml

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-start 

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-define Q-048-1-2.xml

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-start

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-stop

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-stop



Q-073 test
==========

define_system.sh Q-073-1.xml
start_system.sh 
platform_admin.sh --proc-set-info APPL2 P21 @hostname2val@  EXEC=cdmw_test_attributes_managed_process PORT=21882 ARGS="-message=Message2_for_P21"
platform_admin.sh --proc-set-info APPL4 P41 @hostname2val@  EXEC=cdmw_test_attributes_unmanaged_process PORT=21883 ARGS="-message=Message4_for_P41"
platform_admin.sh --proc-initialise APPL4 P41 @hostname2val@
platform_admin.sh --proc-initialise APPL2 P21 @hostname2val@
stop_system.sh 

Q-074 test
==========
define_system.sh Q-074-1-1.xml
start_system.sh 
system_snapshot.sh 
platform_admin.sh --sys-add-host Q-074-2-1.xml
system_snapshot.sh 
platform_admin.sh --sys-add-application Q-074-3-1.xml
platform_admin.sh --app-initialise APPL2
platform_admin.sh --app-run APPL2
system_snapshot.sh 
platform_admin.sh --app-stop APPL2
system_snapshot.sh 
platform_admin.sh --sys-remove-application APPL2
system_snapshot.sh 
platform_admin.sh --sys-remove-host @hostname2val@
system_snapshot.sh 


Q-079 test
==========

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml | tee CdmwValidationTests_Sys1.output

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_Sys2_conf.xml | tee CdmwValidationTests_Sys2.output

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_Sys2_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

start_supervisionSys1.sh 

start_supervisionSys2.sh 

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-define Q-079-1-1.xml

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-start 

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-define Q-079-1-2.xml

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-start

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-stop

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-stop


