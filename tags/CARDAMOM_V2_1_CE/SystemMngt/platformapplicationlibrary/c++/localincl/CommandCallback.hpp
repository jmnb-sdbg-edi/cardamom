/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_COMMAND_CALLBACK_HPP 
#define INCL_PLATFORMMNGT_COMMAND_CALLBACK_HPP 

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


enum CallbackStatus
{
    /**
    * The command succeeded.
    */
    CBK_SUCCEED,

    /**
    * The timeout allowed for the command has expired.
    */
    CBK_TIMEOUT,

    /**
    * An error has been reported during the command processing.
    */
    CBK_ERROR
};

/**
* Purpose:
* <p>
* The class for the command callback.
*/
class CommandCallback
{

public:

        /**
        * Purpose:
        * <p> Waits for the command processing during the allowed timeout.
        * 
        * @param allowedTimeout the timeout in ms.
        * @return the status of the command processing.
        */ 
        CallbackStatus wait(size_t allowedTimeout);


        /**
        * Purpose:
        * <p> Returns the information about the error.
        */ 
        const char* getErrorInfo();


        /**
        * Purpose:
        * <p> Destructor
        */ 
        virtual ~CommandCallback() throw();


        /**
        * Purpose:
        * <p> Constructor
        */ 
        CommandCallback();

        /**
        * Purpose:
        * <p> Signals the command processing.
        * 
        * @param error indicates whether an error has been reported during the
        * command processing.
        * @return true if the command processing has been done before the timeout.
        * Otherwise return false.
        */ 
        bool signal(bool error = false);

        /**
        * The information about the error.
        * Should be updated if needed before calling signal().
        */ 
        std::string m_errorInfo;

private:

        /**
        * The mutex used by m_callback_condition and protecting
        * m_timeout_expired.
        */
        OsSupport::Mutex m_mutex;

        /**
        * The condition on the command processing.
        */
        OsSupport::Condition m_callback_condition;
                
        /**
        * Indicates whether mutex is locked.
        */
        bool m_mutexLocked;

        /**
        * Indicates whether the timeout has expired.
        */
        bool m_timeoutExpired;

        /**
        * Indicates whether an error has been reported.
        */
        bool m_errorReported;


};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_COMMAND_CALLBACK_HPP

