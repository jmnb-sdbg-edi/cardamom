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


#ifndef INCL_PLATFORMMNGT_SYSTEM_LOGMESSAGEID_HPP 
#define INCL_PLATFORMMNGT_SYSTEM_LOGMESSAGEID_HPP

namespace Cdmw
{
namespace PlatformMngt
{
    
/**
* Start message identifier
*/
const long MSG_ID_SYSTEM_START  = 500;

/**
* "Invalid options"
*/
const long MSG_ID_SUPERVISION_INVALID_OPTIONS = MSG_ID_SYSTEM_START + 0;

/**
* "Specified port is invalid"
*/
const long MSG_ID_SUPERVISION_INVALID_PORT = MSG_ID_SYSTEM_START + 1;

/**
* "Port not specified"
*/
const long MSG_ID_SUPERVISION_PORT_NOT_SPECIFIED = MSG_ID_SYSTEM_START + 2;
    
/**
* "Port is already used"
*/
const long MSG_ID_SUPERVISION_UNAVAILABLE_PORT = MSG_ID_SYSTEM_START + 3;

/**
* "Specified event notifier timeout is invalid"
*/
const long MSG_ID_SUPERVISION_INVALID_EVNT_TIMEOUT = MSG_ID_SYSTEM_START + 4;

/**
* "Event notifier timeout not specified"
*/
const long MSG_ID_SUPERVISION_EVNT_TIMEOUT_NOT_SPECIFIED = MSG_ID_SYSTEM_START + 5;
  
/**
* "Specified creation timeout is invalid"
*/
const long MSG_ID_SUPERVISION_INVALID_CREAT_TIMEOUT = MSG_ID_SYSTEM_START + 6;

/**
* "Specified creation timeout not specified"
*/
const long MSG_ID_SUPERVISION_CREAT_TIMEOUT_NOT_SPECIFIED = MSG_ID_SYSTEM_START + 7;
    
/**
* "Log file not specified"
*/
const long MSG_ID_SUPERVISION_LOG_FILE_NOT_SPECIFIED = MSG_ID_SYSTEM_START + 8;
    
/**
* "Supervision started"
*/
const long MSG_ID_SUPERVISION_STARTUP = MSG_ID_SYSTEM_START + 9;

/**
* "Reference published with corbaloc"
*/
const long MSG_ID_SUPERVISION_CORBALOC = MSG_ID_SYSTEM_START + 10;
    
/**
* "Log written in file"
*/
const long MSG_ID_SUPERVISION_LOG_FILE = MSG_ID_SYSTEM_START +11;

/**
* "RunSetManager runs out of memory"
*/
const long MSG_ID_RUNSETMANAGER_MEMORY = MSG_ID_SYSTEM_START +12;

/**
* "Define System"
*/
const long MSG_ID_DEFINE_SYSTEM = MSG_ID_SYSTEM_START +13;

/**
* "Add Application"
*/
const long MSG_ID_ADD_APPLICATION = MSG_ID_SYSTEM_START +14;

/**
* "Remove Application"
*/
const long MSG_ID_REMOVE_APPLICATION = MSG_ID_SYSTEM_START +15;

/**
* "Set Init Graph"
*/
const long MSG_ID_SET_INITGRAPH = MSG_ID_SYSTEM_START +16;

/**
* "Set Stop Graph"
*/
const long MSG_ID_SET_STOPGRAPH = MSG_ID_SYSTEM_START +17;

/**
* "Set RunSet"
*/
const long MSG_ID_SET_RUNSET = MSG_ID_SYSTEM_START +18;

/**
* "Remove RunSet"
*/
const long MSG_ID_REMOVE_RUNSET = MSG_ID_SYSTEM_START +19;

/**
* "Remove All RunSet"
*/
const long MSG_ID_REMOVE_ALL_RUNSET = MSG_ID_SYSTEM_START +20;

/**
* "Add Host"
*/
const long MSG_ID_ADD_HOST = MSG_ID_SYSTEM_START +21;

/**
* "Remove Host"
*/
const long MSG_ID_REMOVE_HOST = MSG_ID_SYSTEM_START +22;

/**
* "System Start"
*/
const long MSG_ID_SYS_START = MSG_ID_SYSTEM_START +23;

/**
* "System Resume"
*/
const long MSG_ID_SYS_RESUME = MSG_ID_SYSTEM_START +24;

/**
* "System Acknowledge"
*/
const long MSG_ID_SYS_ACK = MSG_ID_SYSTEM_START +25;

/**
* "System Stop"
*/
const long MSG_ID_SYS_STOP = MSG_ID_SYSTEM_START +26;

/**
* "Start Application Init Sequencer"
*/
const long MSG_ID_START_APP_INIT_SEQ = MSG_ID_SYSTEM_START +27;

/**
* "Resume Application Init Sequencer"
*/
const long MSG_ID_RESUME_APP_INIT_SEQ = MSG_ID_SYSTEM_START +28;

/**
* "Start Application Stop Sequencer"
*/
const long MSG_ID_START_APP_STOP_SEQ = MSG_ID_SYSTEM_START +29;

/**
* "Start Application RunSet"
*/
const long MSG_ID_START_RUNSET = MSG_ID_SYSTEM_START +30;


/**
* "Register observer"
*/
const long MSG_ID_SYS_REG_OBSERVER = MSG_ID_SYSTEM_START +31;

/**
* "UnRegister observer"
*/
const long MSG_ID_SYS_UNREG_OBSERVER = MSG_ID_SYSTEM_START +32;


/**
* "Initialisation of application by sequencer"
*/
const long MSG_ID_SYS_APP_INIT = MSG_ID_SYSTEM_START +33;
    
/**
* "Status ok for initialising application"
*/
const long MSG_ID_SYS_APP_INIT_STATUS_OK = MSG_ID_SYSTEM_START +34;
    
/**
* "Incompatible status for initialising application"
*/
const long MSG_ID_SYS_APP_INIT_STATUS_ERR = MSG_ID_SYSTEM_START +35;


/**
* "System exception during initialisation of application"
*/
const long MSG_ID_SYS_APP_INIT_SYSTEM_ERR = MSG_ID_SYSTEM_START +36;


/**
* "Start processing of application by starter"
*/
const long MSG_ID_SYS_APP_START = MSG_ID_SYSTEM_START +37;
    
/**
* "Status ok for starting application"
*/
const long MSG_ID_SYS_APP_START_STATUS_OK = MSG_ID_SYSTEM_START +38;

/**
* "Incompatible status for starting application"
*/
const long MSG_ID_SYS_APP_START_STATUS_ERR = MSG_ID_SYSTEM_START +39;


/**
* "System exception during starting of application"
*/
const long MSG_ID_SYS_APP_START_SYSTEM_ERR = MSG_ID_SYSTEM_START +40;

/**
* "Stop processing of application by starter"
*/
const long MSG_ID_SYS_APP_STOP = MSG_ID_SYSTEM_START +41;
    
/**
* "Status ok for stopping application"
*/
const long MSG_ID_SYS_APP_STOP_STATUS_OK = MSG_ID_SYSTEM_START +42;

/**
* "Incompatible status for stopping application"
*/
const long MSG_ID_SYS_APP_STOP_STATUS_ERR = MSG_ID_SYSTEM_START +43;


/**
* "System exception during stopping of application"
*/
const long MSG_ID_SYS_APP_STOP_SYSTEM_ERR = MSG_ID_SYSTEM_START +44;


/**
* "Unexpected Exception"
*/
const long MSG_ID_SYS_UNEXPECTED_EXCEPTION = MSG_ID_SYSTEM_START +45;





/**
* End message identifier
*/
const long MSG_ID_SYSTEM_END = MSG_ID_SYSTEM_START + 46;



class SystemLogMessages
{
    public :
    
    static const char** M_systemMessages;
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_SYSTEM_LOGMESSAGEID_HPP

