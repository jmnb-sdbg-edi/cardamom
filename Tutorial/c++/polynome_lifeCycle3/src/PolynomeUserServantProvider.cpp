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


#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <LifeCycle/lifecycle/LifeCycleObjectBase.hpp>


#include "PolynomeUserServantProvider.hpp"
#include "NoRootsBinome_impl.hpp"
#include "SingleRootBinome_impl.hpp"
#include "TwoRootsBinome_impl.hpp"


namespace 
{
    const char* POLYNOME_REP_ID = "IDL:thalesgroup.com/PolynomeModule/Polynome:1.0";
};



namespace Cdmw
{
namespace Tutorial
{



PolynomeUserServantProvider::~PolynomeUserServantProvider()
    throw()
{
}



PolynomeUserServantProvider::PolynomeUserServantProvider (
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



PortableServer::ObjectId* PolynomeUserServantProvider::create_Polynome ()
    throw ()
{
    // 
    // get the equation coefs
    //
    std::cout << "Enter the coefs of polynome ax*x + bx + c = 0" 
              << std::endl;
    
    m_a=0;
    while (m_a == 0)
    {
        std::cout << "Coef a = ";
        std::cin >> m_a;
        std::cout << std::endl;
                    
        if (m_a == 0)
        {
            std::cout << ">>>>> Coef a cannot be null" << std::endl;
            std::cout << std::endl;
        }
    }
    
    std::cout << "Coef b = ";
    std::cin >> m_b;
    std::cout << std::endl;
    
    std::cout << "Coef c = ";
    std::cin >> m_c;
    std::cout << std::endl;
    
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

    ostr << "POLY_" << oid_counter << "_" << timeval.seconds;
    
	PortableServer::ObjectId_var oid = 
	     PortableServer::string_to_ObjectId(ostr.str().c_str());
	                                       
	
   	std::cout << "<<PolynomeUserServantProvider>> Polynome '" 
   	          << ostr.str().c_str() <<"' OID created" << std::endl;
	
	return oid._retn();
}



PortableServer::Servant 
PolynomeUserServantProvider::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{
    PortableServer::Servant servant;

    //
    // Create implementation object 
    //
    
    // compute the discriminant Delta = b*b - 4*a*c
    double delta = m_b*m_b - 4*m_a*m_c;
    
    // if delta is negative : no roots
    if (delta < 0)
    {
        NoRootsBinome_impl* p_binome = new NoRootsBinome_impl (m_object_deactivator);
            
        servant = p_binome;
    
        std::cout << "<<PolynomeUserServantProvider>> NoRootsBinome_impl servant created" << std::endl;
    }
    
    // else if delta is zero : only one root
    else if (delta == 0)
    {
        SingleRootBinome_impl* p_binome = 
             new SingleRootBinome_impl (m_object_deactivator,m_a,m_b);
            
        servant = p_binome;
    
        std::cout << "<<PolynomeUserServantProvider>> SingleRootBinome_impl servant created" << std::endl;
    }
    
    // else delta is positive : two roots
    else
    {
        TwoRootsBinome_impl* p_binome = 
              new TwoRootsBinome_impl (m_object_deactivator,m_a,m_b,delta);
                      
        servant = p_binome;
    
        std::cout << "<<PolynomeUserServantProvider>> TwoRootsBinome_impl servant created" << std::endl;
    }
    
    // return the servant reference of object
    return servant;
}


void PolynomeUserServantProvider::release_servant(
                     const PortableServer::ObjectId& oid,
                     PortableServer::POA_ptr poa,
                     PortableServer::Servant servant) 
    throw(CORBA::SystemException)
{            
    servant->_remove_ref();
        
    std::cout << "<<PolynomeUserServantProvider>> Polynome servant reference removed." << std::endl;
}



std::string PolynomeUserServantProvider::get_supported_interface(
                         const PortableServer::ObjectId& oid) const
    throw(CORBA::OBJECT_NOT_EXIST)
{
	std::string result = POLYNOME_REP_ID;
            
    return result;    
}



        
}; // End namespace Tutorial
}; // End namespace Cdmw
