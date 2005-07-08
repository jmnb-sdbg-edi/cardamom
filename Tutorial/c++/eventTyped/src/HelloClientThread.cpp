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


#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>


#include <Foundation/common/Locations.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Event/eventinterface/TypedEventChannel.hpp>

#include <TraceAndPerf/tracelibrary/Trace.hpp>


#include "HelloClientProcessControl.hpp"
#include "HelloClientThread.hpp"

namespace 
{

    const char* COMPLETE_TYPED_CHANNEL_NAME = "EVENT_CHANNELS/TypedEventChannel1";
    
    const char* INTERFACE_REF="IDL:TypedEcho:1.0";
 
}; // End anonymous namespace




namespace Cdmw
{
namespace Tutorial
{




HelloClientThread::HelloClientThread(CORBA::ORB_ptr orb, 
                           std::string& applicationName,
                           std::string& processName)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_applicationName (applicationName),
      m_processName (processName),
      m_exit_required(false)
{
    // =======================================================
    // Get NamingInterface object from RepositoryInterface
    // =======================================================
    Cdmw::NamingAndRepository::NamingInterface ni = 
                    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
                                 Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);
                                 
                                 
    // =====================================    
    // Get Event service naming context
    // =====================================
    CORBA::Object_var obj = ni.resolve("EVENT");
    CosNaming::NamingContext_var nc =
            CosNaming::NamingContext::_narrow(obj.in());
       
     
    // ================================================== 
    // Get event channel of server and its push consummer 
    // ==================================================
    if (CORBA::is_nil(nc.in())) 
    {
        std::cerr << "NamingContext is a nil object reference" << std::endl;                      
    }
    else
    {       
        std::auto_ptr <Cdmw::Event::TypedEventChannel> p_typedeventchannel (
                                new Cdmw::Event::TypedEventChannel (COMPLETE_TYPED_CHANNEL_NAME,
                                                                     nc.in()));
                                             
        try 
        {
            m_pushConsumer = 
               p_typedeventchannel->get_typedPushConsumer<TypedEcho> (
                                INTERFACE_REF, CosEventComm::PushSupplier::_nil());
        } 
        catch (...) 
        {
            std::cerr << "impossible to get a push consumer on " << COMPLETE_TYPED_CHANNEL_NAME << std::endl;
        }
    }
}



HelloClientThread::~HelloClientThread() throw()
{
}

 

void HelloClientThread::run () throw()
{
    try
    {        
        std::cout << "Hello Client Thread Started" << std::endl;
        
        std::string baseMsg = "process <" + m_applicationName + "/" + m_processName + ">" ;
        
        // ===================================================
        // trace a message (use CDMW_TRACE in Cardamom thread)
        // ===================================================
        std::string msg = baseMsg + "Client Thread started";  
                                       
        CDMW_TRACE("TUTOR", 1, msg.c_str());



        // ====================================================
        // thread loop
        // ====================================================
        
        while (!m_exit_required) 
        {
            // ===================================================
            // Push event in Push Consumer
            // ===================================================
            if (!CORBA::is_nil(m_pushConsumer.in()))
	        {   
                m_pushConsumer->echo_string("<<**** HELLO ****>>");
	        }


            // ===================================================
            // set thread sleeping
            // ===================================================
            Cdmw::OsSupport::OS::sleep (10000);
        } 
    }
    catch (...)
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "Unexpected exception in thread loop" << std::endl;
    }
}




}; // namespace Tutorial
}; // namespace Cdmw
