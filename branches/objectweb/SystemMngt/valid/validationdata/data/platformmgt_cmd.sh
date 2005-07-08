#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES 
# and SELEX-SI. 
# 
# It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
# All rights reserved.
# 
# CARDAMOM is free software; you can redistribute it and/or modify it under 
# the terms of the GNU Library General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your 
# option) any later version. 
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
# License for more details. 
# 
# You should have received a copy of the GNU Library General 
# Public License along with CARDAMOM; see the file COPYING. If not, write to 
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

Q-022 test
==========
define_system.sh Q-022-1-1.xml
start_system.sh
platform_admin.sh --sys-add-application Q-022-2-1.xml
platform_admin.sh --app-initialise APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --app-add-process APPL4 Q-022-3-1.xml
platform_admin.sh --proc-initialise APPL4 P42
platform_admin.sh --proc-run APPL4 P42
platform_admin.sh --proc-stop APPL3 P32
platform_admin.sh --proc-stop APPL4 P41 EMERGENCY
platform_admin.sh --app-stop APPL4
platform_admin.sh --app-stop APPL3 EMERGENCY
stop_system.sh
platform_admin.sh --host-shutdown=anakin
platform_admin.sh --host-reboot=anakin


Q-023 test
==========
define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13
platform_admin.sh --app-resume APPL1
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh Q-023-2-1.xml
start_system.sh
platform_admin.sh --proc-set-info APPL2 P22 EXEC=cdmw_test_managed_process PORT=21882 ARGS=--nb-steps=1
platform_admin.sh --proc-initialise APPL2 P22
platform_admin.sh --app-resume APPL2
platform_admin.sh --sys-resume
stop_system.sh

define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --sys-add-application Q-023-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41 Q-023-4.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --proc-set-life-cycle APPL4 P41 Q-023-5.xml
platform_admin.sh --proc-initialise APPL4 P41 LAST_STATE 10000 nothing
platform_admin.sh --app-resume APPL4
platform_admin.sh --app-run APPL4
platform_admin.sh --proc-set-life-cycle APPL4 P41 Q-023-6.xml
platform_admin.sh --proc-initialise APPL4 P41 LAST_STATE 10000 nothing
platform_admin.sh --proc-run APPL4 P41
platform_admin.sh --app-acknowledge APPL4
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh Q-023-1-1.xml
start_system.sh
platform_admin.sh --proc-initialise APPL1 P13
platform_admin.sh --proc-next-step APPL1 P13
platform_admin.sh --proc-run APPL1 P13
platform_admin.sh --app-acknowledge APPL1
platform_admin.sh --sys-acknowledge
stop_system.sh

start_system.sh
platform_admin.sh --proc-initialise APPL1 P13
platform_admin.sh --proc-initialise APPL1 P14
platform_admin.sh --proc-next-step APPL1 P13
platform_admin.sh --proc-next-step APPL1 P14
platform_admin.sh --proc-run APPL1 P13
platform_admin.sh --proc-run APPL1 P14
platform_admin.sh --app-acknowledge APPL1
platform_admin.sh --sys-acknowledge
stop_system.sh

define_system.sh Q-023-1-1.xml
platform_admin.sh --sys-add-application Q-023-3-1.xml
platform_admin.sh --proc-set-life-cycle APPL4 P41 Q-023-7.xml
platform_admin.sh --app-initialise APPL4 LAST_STATE 10000 nothing
platform_admin.sh --app-run APPL4
platform_admin.sh --app-stop APPL4

