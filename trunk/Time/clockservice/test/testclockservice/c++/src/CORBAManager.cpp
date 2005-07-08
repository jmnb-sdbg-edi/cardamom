/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
* @brief
*
* @author Fabrizio Morciano <fmorciano@amsjv.it>
* @author Raffaele Mele <lellomele@yahoo.com> 
*/
#include "Time/testclockservice/CORBAManager.hpp"
#include <iostream>
#include <string>
#include "Foundation/common/Assert.hpp"

#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>


#include "tao/PortableServer/POA.h"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"

using namespace std;
using namespace Cdmw;

///////////////////////////////////////////////////////////////////////////////

CORBAManager::CORBAManager()
{
    is_initialized_ = false;
    argc_ = 0;
}

///////////////////////////////////////////////////////////////////////////////

CORBAManager::~CORBAManager()
{
    while(!argv_.empty())
    {
        delete [] argv_.back();
        argv_.pop_back();
    }
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Object_ptr 
CORBAManager::get_object(const std::string& name )
throw (CORBAManager::InvalidObject)
{
    if( !is_initialized_ ) 
    {
        std::cerr<<"Orb is non initialized!"<<endl;
        throw CORBAManager::InvalidObject();
    }
    
    db_iterator it = db_.find(name);
    if( it != db_.end() )
        return it->second;
    
    throw CORBAManager::InvalidObject();
}

///////////////////////////////////////////////////////////////////////////////

CORBA::ORB_var 
CORBAManager::get_orb()
{
    if( !is_initialized_ ) 
    {
        std::cerr<<"Orb is non initialized!"<<endl;
        throw CORBAManager::InvalidObject();
    }
    
    return orb_;
}

///////////////////////////////////////////////////////////////////////////////

void 
CORBAManager::init(int ac, char ** &av )
{
    if(!is_initialized_)
    {
        argc_ = ac;
        int count = 0;
        while( count< ac )
        {
            int len = strlen(av[count]) + 1;
            char * tmp = new char[len];
            strcpy(tmp,av[count]);
            argv_.push_back(tmp);
            ++count;
        }

        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();

        orb_ =OrbSupport::OrbSupport::ORB_init(argc_,
                                    &argv_[0],
                                    strategyList);
                
        is_initialized_=true;
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
CORBAManager::add(const std::string& service_alias )
{
    if( !is_initialized_ ) 
    {
        std::cerr<<"Orb is non initialized!"<<endl;
        throw CORBAManager::InvalidObject();
    }
    
    db_iterator it = db_.find(service_alias);
    if( it == db_.end() )
    {
        CORBA::Object_ptr obj=0;
        try
        {
            obj = orb_->resolve_initial_references( service_alias.c_str() );
        }
        catch (...)
        {
            std::cerr << "PANIC: Unable to resolve:" << service_alias<< endl;
            exit(1);
        }      
        
        try
        {
            if (CORBA::is_nil(obj))
                throw InvalidObject();
            db_[service_alias] = obj;    
        }      
        catch(...)    
        {
            std::cerr<<"Error not valid object for :"<<service_alias<<endl;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void    CORBAManager::activate_poa()
{
    CORBA::Object_var obj=
        CORBAManager::instance()->get_orb()->\
        resolve_initial_references("RootPOA");
    
    CDMW_ASSERT(!CORBA::is_nil(obj.in()));
    
    PortableServer::POA_var rootPOA =
        PortableServer::POA::_narrow(obj.in());
    
    PortableServer::POAManager_var poa_manager =
        rootPOA->the_POAManager();
    
    poa_manager->activate();
}

///////////////////////////////////////////////////////////////////////////////

CORBAManager* 
CORBAManager::instance()
{
    static CORBAManager singleton;
    return &singleton;
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
