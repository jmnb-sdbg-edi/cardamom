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

V-SMG-002-x-1 test
==========
define_system.sh V-SMG-002-1-1.xml 
start_system.sh
platform_admin.sh --sys-add-application V-SMG-002-2-1.xml 
platform_admin.sh --app-initialise APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --app-add-process APPL4 V-SMG-002-3-1.xml
platform_admin.sh --proc-initialise APPL4 P42 @hostname1val@
platform_admin.sh --proc-run APPL4 P42 @hostname1val@
platform_admin.sh --proc-stop APPL3 P32 @hostname2val@
platform_admin.sh --proc-stop APPL4 P41 @hostname1val@ EMERGENCY
platform_admin.sh --app-stop APPL4
platform_admin.sh --app-stop APPL3 EMERGENCY
stop_system.sh
platform_admin.sh --proc-initialise APPL1 P12 @hostname1val@
unregister_observer.sh
platform_admin.sh --proc-stop APPL1 P12 @hostname1val@
platform_admin.sh --host-shutdown=anakin
platform_admin.sh --host-reboot=anakin

V-SMG-002-x-3 test
==========
define_system.sh V-SMG-002-1-3.xml 
start_system.sh
platform_admin.sh --sys-add-application V-SMG-002-2-3.xml 
platform_admin.sh --app-initialise APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --app-add-process APPL4 V-SMG-002-3-3.xml
platform_admin.sh --proc-initialise APPL4 P42 @hostname1val@
platform_admin.sh --proc-run APPL4 P42 @hostname1val@
platform_admin.sh --proc-stop APPL3 P32 @hostname2val@
platform_admin.sh --proc-stop APPL4 P41 @hostname1val@ EMERGENCY
platform_admin.sh --app-stop APPL4
platform_admin.sh --app-stop APPL3 EMERGENCY
stop_system.sh
platform_admin.sh --host-shutdown=anakin
platform_admin.sh --host-reboot=anakin



V-SMG-003 test
==========
define_system.sh V-SMG-003-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13 @hostname2val@
platform_admin.sh --app-resume APPL1
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh V-SMG-003-2-1.xml
start_system.sh
platform_admin.sh --proc-set-info APPL2 P22 @hostname1val@ EXEC=cdmw_test_managed_process PORT=21882 ARGS=--nb-steps=1
platform_admin.sh --proc-initialise APPL2 P22 @hostname1val@
platform_admin.sh --app-resume APPL2
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh V-SMG-003-1-1.xml
start_system.sh
platform_admin.sh --sys-add-application V-SMG-003-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ V-SMG-003-4.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ V-SMG-003-5.xml
platform_admin.sh --proc-initialise APPL4 P41 @hostname1val@ LAST_STATE 10000 nothing
platform_admin.sh --app-resume APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ V-SMG-003-6.xml
platform_admin.sh --proc-initialise APPL4 P41 @hostname1val@ LAST_STATE 10000 nothing
platform_admin.sh --proc-run APPL4 P41 @hostname1val@
platform_admin.sh --app-acknowledge APPL4
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh V-SMG-003-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13 @hostname2val@
platform_admin.sh --proc-next-step APPL1 P13 @hostname2val@
platform_admin.sh --proc-run APPL1 P13 @hostname2val@
platform_admin.sh --app-acknowledge APPL1
platform_admin.sh --sys-acknowledge


platform_admin.sh --proc-initialise APPL1 P11 @hostname1val@
platform_admin.sh --proc-initialise APPL1 P12 @hostname1val@
platform_admin.sh --proc-next-step APPL1 P11 @hostname1val@
platform_admin.sh --proc-run APPL1 P11 @hostname1val@
platform_admin.sh --proc-run APPL1 P12 @hostname1val@
platform_admin.sh --app-acknowledge APPL1

platform_admin.sh --app-stop APPL2
platform_admin.sh --app-initialise APPL2
platform_admin.sh --app-run APPL2
platform_admin.sh --app-acknowledge APPL2
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh V-SMG-003-1-1.xml
platform_admin.sh --sys-add-application V-SMG-003-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41 @hostname1val@ V-SMG-003-7.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --app-run APPL4
platform_admin.sh --app-stop APPL4


V-SMG-010 test
==========
cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml | tee CdmwValidationTests_Sys1.output

cdmw_platform_daemon.sh --CdmwXMLFile=CdmwPlatformMngtDaemon_Sys2_conf.xml | tee CdmwValidationTests_Sys2.output

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

cdmw_ft_manager --CdmwXMLFile=CdmwFaultToleranceManager_Sys2_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml 

start_supervisionSys1.sh 

start_supervisionSys2.sh 

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_supervision_observer --register --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --observer-name=observer

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-define V-SMG-010-1-1.xml 
cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-start 

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-define V-SMG-010-1-2.xml 
cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-start

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-snapshot

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-stop

cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21969/CdmwPlatformMngtSupervision --sys-stop


