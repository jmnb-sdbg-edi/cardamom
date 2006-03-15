/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
*/
/* ===================================================================== */


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <FaultTolerance/ftrequestlogging/FTReplyRecording.hpp>

#include "testftrequestlogging/TestHello.skel.hpp"

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

