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


#ifndef INCL_PLATFORMMNGT_SIM_PROCESSOBSERVER_HPP 
#define INCL_PLATFORMMNGT_SIM_PROCESSOBSERVER_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include <fstream>

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose:
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class SIM_ProcessObserver : public virtual POA_CdmwPlatformMngt::ProcessObserver,
    virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        SIM_ProcessObserver(const char* logFile);


        /**
        * Purpose:
        * <p> Stops the observer.
        * Closes the log file.
        */ 
        void stop();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~SIM_ProcessObserver()
                throw();

        virtual void notify_user_event(const char* process_name,
                const char* issuer,
                const CORBA::Any& data)
            throw(CORBA::SystemException);

        virtual void notify_message(const char* process_name,
                const CdmwPlatformMngtBase::EventHeader& header,
                const char* issuer, const char* message) 
            throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException);
        
        virtual void notify_error(const char* process_name,
                const CdmwPlatformMngtBase::TimeStamp& time_stamp,
                const char* issuer, const char* error_info) 
            throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException);

        virtual void notify_ending(const char* process_name,
                const CdmwPlatformMngtBase::TimeStamp& time_stamp)
            throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException);

        virtual void notify_monitoring_failure(const char* process_name,
                const CdmwPlatformMngtBase::TimeStamp& time_stamp)
            throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException);

        virtual void notify_point_monitoring_failure(const char* process_name,
                CORBA::ULong point_index,
                const CdmwPlatformMngtBase::TimeStamp& time_stamp)
            throw(CdmwPlatformMngt::ProcessNotFound,
                CdmwPlatformMngt::ActivityPointNotFound,
                CORBA::SystemException);


    protected:


    private:


        /**
         * The log file stream
         */
        std::ofstream m_fs;


}; // End class SIM_ProcessObserver 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif


