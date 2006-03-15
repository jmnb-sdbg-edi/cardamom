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


#ifndef INCL_TEST_OBSERVER_IMPL_HPP 
#define INCL_TEST_OBSERVER_IMPL_HPP

#include <iostream>
#include <map>

#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <ftrequestlogging/CdmwFTRequestInfo.stub.hpp>
#include <testftrequestlogging/TestHello_impl.hpp>


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
        Observer_impl(CORBA::ORB_ptr orb, ::Cdmw::HelloInterface_impl& hello)
           : m_insert_counter(0),
             m_hello(hello)
        {
            m_orb = CORBA::ORB::_duplicate(orb);
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

           m_insert_counter++;
           std::cout << "*********** Observer => insert_counter = " 
                     << m_insert_counter 
                     << std::endl;
           
           Cdmw::OsSupport::OS::Timeval time = Cdmw::OsSupport::OS::get_time();
           m_insert_time.insert(TimeMap::value_type(oid, time));
           /*std::string host_name = Cdmw::OsSupport::OS::get_hostname();

           if ((host_name == "padmee") &&
               (insert_counter == 6))
           {
               OsSupport::OS::sleep(1000);
               CDMW_ASSERT(false);
           }*/
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

           TimeMap::iterator iter;

           iter = m_insert_time.find(oid);
           if (iter != m_insert_time.end())
           {
               Cdmw::OsSupport::OS::Timeval remove_time = 
                  Cdmw::OsSupport::OS::get_time();
               Cdmw::OsSupport::OS::Timeval insert_time = iter->second;

               CORBA::ULong remove_time_us = remove_time.seconds * 1000000 +
                  remove_time.microseconds;

               CORBA::ULong insert_time_us = insert_time.seconds * 1000000 +
                  insert_time.microseconds;

               CORBA::ULong duration = remove_time_us - insert_time_us;
               std::cout << "retention period for oid= " 
                         << oid 
                         << "is " 
                         << duration
                         << "us"
                         << std::endl;
               m_hello.set_recorded_duration(duration);
           }
           else
           {
               // could not appear!
               CDMW_ASSERT(false);
           }
           m_insert_time.erase(oid);
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

        int m_insert_counter;

        // map containing for record oid the date of insertion in the datastore
        TimeMap m_insert_time;

        Cdmw::HelloInterface_impl& m_hello;

    }; // End class Observer_impl


}; // End namespace Cdmw

#endif // INCL_TEST_OBSERVER_IMPL_HPP

