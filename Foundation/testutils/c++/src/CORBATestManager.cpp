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

#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/common/Assert.hpp"
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::CORBATestManager* Cdmw::TestUtils::CORBATestManager::m_instance = 0;
 
///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::CORBATestManager::CORBATestManager()
:Cdmw::TestUtils::TestManager()
{
}

///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::CORBATestManager::~CORBATestManager()
{
}

///////////////////////////////////////////////////////////////////////////////

CORBA::ORB_ptr
Cdmw::TestUtils::CORBATestManager::get_ORB()
{
    if( !m_is_initialized ) 
    {
        std::cerr<<"Orb is non initialized!"<<std::endl;
        throw Cdmw::TestUtils::CORBATestManager::InvalidObject();
    }
    
    return m_orb;
}

///////////////////////////////////////////////////////////////////////////////

PortableServer::POA_ptr
Cdmw::TestUtils::CORBATestManager::get_POA()
{
    if( !m_is_initialized)
    {
        std::cerr<<"POA is non initialized!"<<std::endl;
        throw Cdmw::TestUtils::CORBATestManager::InvalidObject();
    }
    return m_rootPOA;
}

///////////////////////////////////////////////////////////////////////////////


void 
Cdmw::TestUtils::CORBATestManager::init(CORBA::ORB_ptr orb,
                                        PortableServer::POA_ptr  poa,
                                        int      argc,
                                        char***  argv )
{
    TestManager::init(argc,argv);    
    m_orb =  CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(poa);
}

#if 0
///////////////////////////////////////////////////////////////////////////////

void
Cdmw::TestUtils::CORBATestManager::is_POA_active()
{
    CDMW_ASSERT(!CORBA::is_nil(m_orb));

    //   m_rootPOA = PortableServer::POA::_narrow(obj.in());

    if(CORBA::is_nil(m_orb) ||
       CORBA::is_nil(m_rootPOA))
    {
       m_POA_is_active = false;
    }
    else
    {
        m_poa_manager = m_rootPOA->the_POAManager();
    
        PortableServer::POA::State 
           state = m_poa_manager->get_state();

       if( state == PortableServer::POA::ACTIVE) 
          m_POA_is_active = true;
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::CORBATestManager* 
Cdmw::TestUtils::CORBATestManager::instance()
{
    if (Cdmw::TestUtils::CORBATestManager::m_instance == 0)
        Cdmw::TestUtils::CORBATestManager::m_instance = new Cdmw::TestUtils::CORBATestManager(); 
    return Cdmw::TestUtils::CORBATestManager::m_instance; 
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Object_ptr 
Cdmw::TestUtils::CORBATestManager::get_object(const std::string& name )
throw (Cdmw::TestUtils::CORBATestManager::InvalidObject)
{
    if( !m_is_initialized ) 
    {
        std::cerr<<"Orb is non initialized!"<<std::endl;
        throw Cdmw::TestUtils::CORBATestManager::InvalidObject();
    }
    
    db_iterator it = m_db.find(name);

    if (it != m_db.end())                                               
    {                                                                  
        CORBA::Object_var ret = CORBA::Object::_duplicate(it->second); 
        return ret._retn();                                            
    }                                                                  
    
    throw Cdmw::TestUtils::CORBATestManager::InvalidObject();
}


///////////////////////////////////////////////////////////////////////////////

void  
Cdmw::TestUtils::CORBATestManager::add(const std::string& service_alias )
throw (InvalidObject)
{
    if( !m_is_initialized ) 
    {
        std::cerr<<"Orb is non initialized!"<<std::endl;
        throw Cdmw::TestUtils::CORBATestManager::InvalidObject();
    }

    db_iterator it = m_db.find(service_alias);

    if( it == m_db.end() )
    {
        CORBA::Object_var obj;
        obj = m_orb->resolve_initial_references( service_alias.c_str() );
        
        if (CORBA::is_nil(obj.in() ))
            throw InvalidObject();

        m_db[service_alias] = CORBA::Object::_duplicate(obj.in()); 
    }
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
