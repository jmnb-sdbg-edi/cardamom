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


#ifndef INCL_HELLO_LOGGER_IMPL_HPP 
#define INCL_HELLO_LOGGER_IMPL_HPP 

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "Foundation/osthreads/Mutex.hpp"
#include <FaultTolerance/ftrequestlogging/FTReplyRecording.hpp>

#include "testftrequestlogging2/TestHello.skel.hpp"

#include <string>

namespace Cdmw
{
    class Observer_impl;

    class HelloLogger_impl:
            public virtual POA_CdmwRequestLogging::HelloLogger,
            virtual public PortableServer::RefCountServantBase
    {

    public:
        
        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */ 
        HelloLogger_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            throw(CORBA::SystemException);
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~HelloLogger_impl()
            throw();
        
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual CdmwRequestLogging::LogSeq*
        get_log_traces ()
            throw(CORBA::SystemException);


        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */
        virtual void set_log_trace(const char* trace)
            throw(CORBA::SystemException);


    private:

        // Hide copy constructor/assignment operator
        HelloLogger_impl(const HelloLogger_impl& rhs)
            throw();
        
        HelloLogger_impl&
        operator=(const HelloLogger_impl& rhs)
            throw();

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

        CdmwRequestLogging::LogSeq_var m_log_traces;
        OsSupport::Mutex m_mutex;  // mutex used to protect m_log_traces
    }; // End class HelloLogger_impl 


}; // End namespace Cdmw

#endif // INCL_HELLO_LOGGER_IMPL_HPP 

