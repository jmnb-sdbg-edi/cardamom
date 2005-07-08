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


#include <cstdlib>
#include <string>
#include <sstream>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <LifeCycle/lifecycle/LifeCycleObjectBase.hpp>


#include "PrinterUserServantProvider.hpp"
#include "Printer_impl.hpp"

namespace 
{
    const char* PRINTER_REP_ID = "IDL:thalesgroup.com/PrinterModule/Printer:1.0";
};



namespace Cdmw
{
namespace Tutorial
{



PrinterUserServantProvider::~PrinterUserServantProvider()
    throw()
{
}



PrinterUserServantProvider::PrinterUserServantProvider (
                   CORBA::ORB_ptr orb,
                   PortableServer::POA_ptr poa,
                   const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                   int & argc, char** argv)
  throw(CORBA::SystemException)
  
  : m_orb(CORBA::ORB::_duplicate(orb)),
    m_poa(PortableServer::POA::_duplicate(poa)),
    m_object_deactivator(deactivator)
{
    m_counter=0L;
}



PortableServer::ObjectId* PrinterUserServantProvider::create_printer (const char* device)
    throw ()
{
    //
    // create an unique oid
    //
    
    unsigned long oid_counter;
    
    {
        const unsigned long MAX_COUNTER = 0xFFFFFFFF;
        
        CDMW_MUTEX_GUARD(m_count_mutex);
        
        if (m_counter == MAX_COUNTER)
        {
            m_counter = 0L;
        }
        else
        {
            m_counter++;
        }
        
        oid_counter = m_counter;
    }
    
    OsSupport::OS::Timeval timeval = OsSupport::OS::get_time();
    
    std::stringstream ostr;

    ostr << "Printer_bw_" << device << "_" << oid_counter << "_" << timeval.seconds;
    
	PortableServer::ObjectId_var oid = 
	         PortableServer::string_to_ObjectId(ostr.str().c_str());
	
   	std::cout << "<<PrinterUserServantProvider>> Printer '" 
   	          << ostr.str().c_str() <<"' OID created" << std::endl;
	
	return oid._retn();
}



PortableServer::Servant 
PrinterUserServantProvider::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{
    //
    // Create implementation object 
    //
        
    Printer_impl* printer_impl = 
                   new Printer_impl(m_object_deactivator);
    
    PortableServer::Servant servant = printer_impl;
    
    std::cout << "<<PrinterUserServantProvider>> Printer servant created" << std::endl;

    // return the servant reference of object
    return servant;
}


void PrinterUserServantProvider::release_servant(
                     const PortableServer::ObjectId& oid,
                     PortableServer::POA_ptr poa,
                     PortableServer::Servant servant) 
    throw(CORBA::SystemException)
{           
    servant->_remove_ref();
        
    std::cout << "<<PrinterUserServantProvider>> Printer servant reference removed." << std::endl;
}



std::string PrinterUserServantProvider::get_supported_interface(
                         const PortableServer::ObjectId& oid) const
    throw(CORBA::OBJECT_NOT_EXIST)
{
	std::string result = PRINTER_REP_ID;
            
    return result;    
}



        
}; // End namespace Tutorial
}; // End namespace Cdmw
