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


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <FaultTolerance/ftrequestlogging/FTReplyRecording.hpp>

#include "testftrequestlogging3/TestHello.skel.hpp"

#include <string>

namespace Cdmw
{
    class Observer_impl;

    class HelloInterface_impl:
            public virtual POA_CdmwRequestLogging::HelloInterface,
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
        HelloInterface_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            throw(CORBA::SystemException);
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~HelloInterface_impl()
            throw();
        
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void hello ()
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual CORBA::Long 
        do_something (CORBA::Long val)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        char * do_something_more (char *& val, 
                                  ::CdmwRequestLogging::MyStruct_out val_struct)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual CORBA::Long 
        get_log_size ()
            throw(CORBA::SystemException);


        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual CdmwRequestLogging::TimeSeq*
        get_recorded_durations ()
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual bool
        send_bad_context_exception ()
            throw(CORBA::SystemException);


        void set_recorded_duration(CORBA::ULong duration_in_us);


    private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
            throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
            throw();

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

        Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore* m_store;
        Observer_impl* m_observer;

        CdmwRequestLogging::TimeSeq_var m_recorded_durations;
        OsSupport::Mutex m_mutex; // mutex to protect access to m_recorded_durations
        
        std::string      m_hostname;
    }; // End class HelloInterface_impl 




}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

