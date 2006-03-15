/* =========================================================================== *
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
 * =========================================================================== */

#include <testnotificationreception/TestNotificationReception.hpp>
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Exception.hpp>
#include <list>
#include <sstream>


namespace {
    
    const unsigned long MAX_NOTIFICATION = 100;
}

namespace TestSupervision
{
    class SupervisionObserver: public virtual POA_CdmwPlatformMngt::SupervisionObserver,
                               public virtual PortableServer::RefCountServantBase
    {
        typedef std::list<long> MyList;

    public:
        SupervisionObserver()
        {
            m_list = new MyList();
            first = 0;
            end = 0;
            first_is_set = false;
            
        }
        ~SupervisionObserver()
        {
            delete m_list;
        }
        
        void notify( CdmwPlatformMngt::Event* an_event )
            throw( CORBA::SystemException )
        {
            CORBA::String_var message = an_event->to_string();
            std::string mes=CORBA::string_dup(message);
            std::cout<<mes<<std::endl;
            Cdmw::OsSupport::OS::sleep (300);
            std::string::size_type pos = mes.rfind(":");
            std::string value = mes.substr(pos + 1, mes.size());
            long tmp = ::atoi(value.c_str());
            if (first_is_set == false)
            {
                first = tmp;
                first_is_set = true;
                
            }
            end = tmp;
            
            m_list->push_back(tmp);
        }

        bool test_ok()
        {
            m_list->sort();
            if ((end - first) > 0)
            {
                if (abs(end - first) < (m_list->size()))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        
        bool test_finished()
        {
            std::cout<<end <<" "<<MAX_NOTIFICATION<<std::endl;
            if (end == MAX_NOTIFICATION)
                return true;
            else
                return false;
        }
        
        
    private :
        MyList* m_list;
        unsigned long first;
        unsigned long end;
        bool first_is_set;
        
        
    };


        
// constructor
TestNotificationReception::TestNotificationReception(CORBA::ORB_ptr orb,
                                                     const std::string& name,
                                                     const std::string& function)
    : Testable(name),
      m_function(function),
      m_stop(false)
{
    m_orb = CORBA::ORB::_duplicate(orb);
}


// destructor
TestNotificationReception::~TestNotificationReception() 
{};



// do_tests

void TestNotificationReception::do_tests()
{
    bool test_ok = true;
    
    if (m_function == "notificator")
    {
        TEST_INFO("Start test Notifier");
        set_nbOfRequestedTestOK (1);
        unsigned long inc = 0;
        Cdmw::OsSupport::OS::sleep (2000);
        while (!m_stop)
        {
            inc++;
            try
            {                
                std::ostringstream inc_stream;
                inc_stream << inc;
                std::cout<<"notify message "<<inc<<std::endl;
                Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                                      "Notifier", inc_stream.str().c_str());
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cerr << ex <<std::endl;
                test_ok = false;
            }
            catch(...)
            {
                test_ok = false;
            }
            Cdmw::OsSupport::OS::sleep (50);
            if (inc  == 20)
            {
                std::cout<<"Kill the supervision"<<std::endl;
                Cdmw::OsSupport::OS::create_process("test_kill_supervision.sh", "");
            }
    
            if (inc  == MAX_NOTIFICATION)
                m_stop = true;
        }
        
        if (test_ok)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
    } else {
        TEST_INFO("Start test Receptor");
        set_nbOfRequestedTestOK (1);

        TEST_INFO("create the observer and register it");  
        SupervisionObserver* observer_impl = new SupervisionObserver();
        CdmwPlatformMngt::SupervisionObserver_var observer = observer_impl->_this();

    
        CORBA::Object_var system_obj
            = m_orb->string_to_object("corbaloc::localhost:21869/CdmwPlatformMngtSupervision");

        if (CORBA::is_nil (system_obj.in()))
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid corbaloc" );
    
        CdmwPlatformMngt::System_var system = CdmwPlatformMngt::System::_narrow (system_obj.in());

        if (CORBA::is_nil (system.in()))
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid system reference" );

        system->register_observer ("my_observer", observer.in());
        
        while (!observer_impl->test_finished())
            Cdmw::OsSupport::OS::sleep (1000);
        
        TEST_CHECK(observer_impl->test_ok());
    }
}


}


