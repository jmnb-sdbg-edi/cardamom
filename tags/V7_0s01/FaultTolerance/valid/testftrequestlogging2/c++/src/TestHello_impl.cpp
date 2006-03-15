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
#include <iostream>
#include <sstream>

#include "testftrequestlogging2/TestHello_impl.hpp"
#include "testftrequestlogging2/TestObserver.hpp"
#include "ftcommon/ReplyRecording.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"


using namespace std;

namespace Cdmw
{


HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr orb,
                                         PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{

    Cdmw::CommonSvcs::DataStore::DataStoreBase* ds
        = Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
              Cdmw::FT::FT_REPLY_DATASTORE_ID);
    m_store = 
        dynamic_cast<Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore*>(ds);

    // find object group
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

    Cdmw::CommonSvcs::Naming::NamingInterface ni =
           Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");

    std::string logger_full_name = "dom3/HelloLogger";
    CORBA::Object_var obj;
    
    try
    {
        obj = ni.resolve (logger_full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    CdmwRequestLogging::HelloLogger_var hello_logger_ref =
            CdmwRequestLogging::HelloLogger::_narrow(obj.in());
    
    m_observer = new Observer_impl(m_orb.in(), hello_logger_ref.in());
    m_store->register_observer(m_observer);
    
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
   delete m_observer;
}

void HelloInterface_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 1 ######>" << std::endl;
}

CORBA::Long HelloInterface_impl::do_something (CORBA::Long val)
    throw(CORBA::SystemException)
{
    std::cout << "do_something return square of " << val << std::endl;
    return val * val;
}


CORBA::Long HelloInterface_impl::get_log_size ()
    throw(CORBA::SystemException)
{
    return m_store->size();
}

}; // End namespace Cdmw

