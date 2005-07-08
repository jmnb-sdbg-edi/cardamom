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

#include <iostream>

#include <ccm_ft_datastore/CCM_ServerWithEvents_impl.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <ccm_ft_datastore/HostnameResult_impl.hpp>



class SucideThread : public Cdmw::OsSupport::Thread 
{
    public: 
        SucideThread(unsigned int timeToKill)
            : m_timeToKill(timeToKill)
        {};
        
    protected:
        void run() throw()
        {
            Cdmw::OsSupport::OS::sleep(m_timeToKill);
            std::cout << "   ... CCM_ServerWithEvents_impl:  SUICIDE now !!!!" << std::endl;
            abort();
        };
    
    private:
        SucideThread();
        SucideThread(const SucideThread&);
        SucideThread& operator = (const SucideThread&);
   
        unsigned int m_timeToKill;    
};


//
// IDL:thalesgroup.com/CcmFtTest/CCM_ServerWithEvents:1.0
//
CcmFtTest::CCM_ServerWithEvents_impl::CCM_ServerWithEvents_impl()
{
   m_expectedClientHostname = CORBA::string_dup("");
}

CcmFtTest::CCM_ServerWithEvents_impl::~CCM_ServerWithEvents_impl()
{
    std::cout << "   ... CCM_ServerWithEvents_impl::~CCM_ServerWithEvents_impl called!" << std::endl;
}



//
// attribute expectedClientHostname
//
char*
CcmFtTest::CCM_ServerWithEvents_impl::expectedClientHostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedClientHostname);
}

void
CcmFtTest::CCM_ServerWithEvents_impl::expectedClientHostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedClientHostname =  CORBA::string_dup(a);
}


//
// attribute datastoreID
//
CORBA::Long CcmFtTest::CCM_ServerWithEvents_impl::datastoreID ()
    throw (CORBA::SystemException)
{
    return m_datastoreID;
}

void CcmFtTest::CCM_ServerWithEvents_impl::datastoreID (const CORBA::Long a)
    throw (CORBA::SystemException)
{
    m_datastoreID = a;
}


//
// consumer commands
//
void 
CcmFtTest::CCM_ServerWithEvents_impl::push_commands (CcmFtTest::Command * event)
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithEvents_impl::push_commands called with command " << event->commandName() << std::endl;
    
    if (strcmp(event->commandName(), CcmFtTest::COMMAND_KILL) == 0) {
        std::cout << "   ... CCM_ServerWithEvents_impl:  SUICIDE in 500 ms !!!!" << std::endl;
        SucideThread* thread = new SucideThread(500);
        thread->start();
    } else {
        if (strcmp(event->commandName(), CcmFtTest::COMMAND_GET_HOSTNAME) == 0) {
            std::string hostname = Cdmw::OsSupport::OS::get_hostname();
            std::cout << "   ... CCM_ServerWithEvents_impl:  return result " << hostname << std::endl;
            
            CcmFtTest::HostnameResult_var result = new CcmFtTest::HostnameResult_impl();
            result->hostname(CORBA::string_dup(hostname.c_str()));
            
            std::cout << "   ... CCM_ServerWithEvents_impl:  emits HostnameResult " << std::endl;
            m_session_context->push_hostnameEmits(result.in());
        }
        else
            std::cout << "   ... CCM_ServerWithEvents_impl:  Unkown Command !!!!" << std::endl;
    }
};


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithEvents_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithEvents_impl::Set_session_context called!" << std::endl;
   
    m_session_context = ::CcmFtTest::CCM_ServerWithEvents_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithEvents_impl::ccm_activate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithEvents_impl::Ccm_activate called!" << std::endl;
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
CcmFtTest::CCM_ServerWithEvents_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithEvents_impl::Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
void
CcmFtTest::CCM_ServerWithEvents_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithEvents_impl::Ccm_remove called!" << std::endl;
}


