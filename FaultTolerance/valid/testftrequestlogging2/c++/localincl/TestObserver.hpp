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


#ifndef INCL_TEST_OBSERVER_IMPL_HPP 
#define INCL_TEST_OBSERVER_IMPL_HPP

#include <iostream>
#include <map>

#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <ftrequestlogging/CdmwFTRequestInfo.stub.hpp>
#include <testftrequestlogging2/TestHello.stub.hpp>


namespace Cdmw
{
    class Observer_impl : 
       public Cdmw::CommonSvcs::DataStore::DataStoreObserver<std::string, 
                                                             CdmwFT::ReplyData>
    {

    public:
        typedef std::map<std::string, Cdmw::OsSupport::OS::Timeval> TimeMap;

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */        
        Observer_impl(CORBA::ORB_ptr orb, 
                      ::CdmwRequestLogging::HelloLogger_ptr logger)
           : m_hello_logger(::CdmwRequestLogging::HelloLogger::_duplicate(logger)),
             m_do_set_log_trace(true)
        {
            m_orb = CORBA::ORB::_duplicate(orb);
            m_host_name = Cdmw::OsSupport::OS::get_hostname();
        }
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        ~Observer_impl() throw() 
        {
        }
        
    
        void on_insert(_Oid_in_type oid, _Data_in_type data)
            throw ()
        {
           std::cout << "*********** Observer => on_insert called oid="
                     << oid
                     << std::endl;

           std::string trace = m_host_name + " Observer => on_insert called oid=" + oid;
           m_hello_logger->set_log_trace(trace.c_str());
           m_do_set_log_trace = false;
        }

        void on_update(_Oid_in_type oid, _Data_in_type data)
            throw ()
        {
        }

        void on_remove(_Oid_in_type oid)
            throw ()
        {
           std::cout << "*********** Observer => on_remove called oid="
                     << oid
                     << std::endl;
        }

    private:

        // Hide copy constructor/assignment operator
        Observer_impl(const Observer_impl& rhs)
            throw();
        
        Observer_impl&
        operator=(const Observer_impl& rhs)
            throw();
        
        /**
         * The ORB.
         */
        CORBA::ORB_var m_orb;

        CdmwRequestLogging::HelloLogger_var m_hello_logger;
        bool m_do_set_log_trace;

        std::string m_host_name;

    }; // End class Observer_impl


}; // End namespace Cdmw

#endif // INCL_TEST_OBSERVER_IMPL_HPP

