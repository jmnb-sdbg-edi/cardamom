/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef __PLATFORMADMIN_HPP__
#define __PLATFORMADMIN_HPP__

#define LOG_FILE_OPTION        "--log-file"
#define CHECK_ONLY_OPTION      "--check-only"
#define SYSTEM_CORBALOC_OPTION "--system-corbaloc"
#define SYSTEM_PORT_OPTION     "--system-port"

#define SYS_SNAPSHOT           "--sys-snapshot"
#define SYS_DEFINE             "--sys-define"
#define SYS_ADD_HOST           "--sys-add-host"
#define SYS_REMOVE_HOST        "--sys-remove-host"
#define SYS_ADD_APPLICATION    "--sys-add-application"
#define SYS_REMOVE_APPLICATION "--sys-remove-application"
#define SYS_ADD_ENTITY         "--sys-add-entity"
#define SYS_REMOVE_ENTITY      "--sys-remove-entity"
#define SYS_ADD_SERVICE        "--sys-add-service"
#define SYS_REMOVE_SERVICE     "--sys-remove-service"
#define SYS_ADD_PROPERTY       "--sys-add-property"
#define SYS_REMOVE_PROPERTY    "--sys-remove-property"
#define SYS_SET_INIT_GRAPH     "--sys-set-init-graph"
#define SYS_SET_STOP_GRAPH     "--sys-set-stop-graph"
#define SYS_START              "--sys-start"
#define SYS_RESUME             "--sys-resume"
#define SYS_ACKNOWLEDGE        "--sys-acknowledge"
#define SYS_STOP               "--sys-stop"

#define APP_SNAPSHOT           "--app-snapshot"
#define APP_ADD_PROCESS        "--app-add-process"
#define APP_REMOVE_PROCESS     "--app-remove-process"
#define APP_ADD_SERVICE        "--app-add-service"
#define APP_REMOVE_SERVICE     "--app-remove-service"
#define APP_ADD_ENTITY         "--app-add-entity"
#define APP_REMOVE_ENTITY      "--app-remove-entity"
#define APP_ADD_PROPERTY       "--app-add-property"
#define APP_REMOVE_PROPERTY    "--app-remove-property"
#define APP_SET_INIT_GRAPH     "--app-set-init-graph"
#define APP_SET_STOP_GRAPH     "--app-set-stop-graph"
#define APP_INITIALISE         "--app-initialise"
#define APP_RESUME             "--app-resume"
#define APP_ACKNOWLEDGE        "--app-acknowledge"
#define APP_RUN                "--app-run"
#define APP_STOP               "--app-stop"

#define PROC_SNAPSHOT          "--proc-snapshot"
#define PROC_ADD_SERVICE       "--proc-add-service"
#define PROC_REMOVE_SERVICE    "--proc-remove-service"
#define PROC_ADD_ENTITY        "--proc-add-entity"
#define PROC_REMOVE_ENTITY     "--proc-remove-entity"
#define PROC_ADD_PROPERTY      "--proc-add-property"
#define PROC_REMOVE_PROPERTY   "--proc-remove-property"
#define PROC_INITIALISE        "--proc-initialise"
#define PROC_NEXT_STEP         "--proc-next-step"
#define PROC_RUN               "--proc-run"
#define PROC_STOP              "--proc-stop"
#define PROC_SET_INFO          "--proc-set-info"
#define PROC_SET_AUTOENDING    "--proc-set-autoending"
#define PROC_SET_HOST          "--proc-set-host"
#define PROC_SET_LIFE_CYLE     "--proc-set-life-cycle"

#define HOST_REBOOT            "--host-reboot"
#define HOST_SHUTDOWN          "--host-shutdown"


#endif //__PLATFORMADMIN_HPP__

