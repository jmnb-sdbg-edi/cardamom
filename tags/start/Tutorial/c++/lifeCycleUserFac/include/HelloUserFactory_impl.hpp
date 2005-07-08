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

#ifndef INCL_HELLO_FACTORY_IMPL_HPP 
#define INCL_HELLO_FACTORY_IMPL_HPP 


#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <Foundation/orbsupport/POAThreadingPolicy.hpp>
#include <LifeCycle/lifecycle/FactoryAllocator.hpp>


#include "Hello.skel.hpp"


namespace Cdmw
{
namespace Tutorial
{

class HelloInterface_impl :
        public virtual POA_Hello::HelloInterface,
        virtual public PortableServer::RefCountServantBase
{

    public:


        HelloInterface_impl(CORBA::ORB_ptr orb,
                            PortableServer::POA_ptr parent)
            throw(CORBA::SystemException);


        virtual 
        ~HelloInterface_impl()
                throw();
    
    
        virtual PortableServer::POA_ptr _default_POA() 
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }
        
        
        //
        // IDL:thalesgroup.com/Hello/HelloInterface/display_hello:1.0
        //
        virtual void display_hello ()
            throw(CORBA::SystemException);
                  

    private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
                throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
                throw();
                
                
        /**
        * The ORB reference.
        */
        CORBA::ORB_var  m_orb;
    
        /**
        * The Root POA reference.
        */
        PortableServer::POA_var m_poa;
    


}; // End class HelloInterface_impl 







class HelloUserFactory_impl :
        public virtual POA_Hello::HelloUserFactory,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{

public:
    

    HelloUserFactory_impl (CORBA::ORB_ptr  orb, 
                           PortableServer::POA_ptr parent,
                           const std::string& factory_name,
                           Cdmw::OrbSupport::POAThreadingPolicy& threading_policy)
        throw(Cdmw::OutOfMemoryException,
              Cdmw::BadParameterException,
              Cdmw::InternalErrorException,
              CORBA::SystemException);
    
 
    virtual 
    ~HelloUserFactory_impl()
        throw();
    
    
    
    virtual PortableServer::POA_ptr _default_POA() 
    {
        return PortableServer::POA::_duplicate(m_poa.in());
    }
    
    
    
   
    //
    // IDL:thalesgroup.com/Hello/HelloFactory/create_helloInterface:1.0
    //
    virtual Hello::HelloInterface_ptr create_helloInterface ()
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/lifecycle/FactoryBase/supports:1.0
    //
    virtual CORBA::Boolean supports(const char* repository_id)
        throw(CORBA::SystemException);
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/lifecycle/FactoryBase/get_supported_objects:1.0
    //
    virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
        throw(CORBA::SystemException);

    
private:

    HelloUserFactory_impl()
        throw(CORBA::SystemException);
    
    // Hide copy constructor/assignment operator
    HelloUserFactory_impl(const HelloUserFactory_impl& rhs)
        throw();
    
    HelloUserFactory_impl&
    operator=(const HelloUserFactory_impl& rhs)
        throw();


    /**
    * The ORB reference.
    */
    CORBA::ORB_var  m_orb;
    
    /**
    * The Root POA reference.
    */
    PortableServer::POA_var m_poa;
    
    /**
    * The Object POA reference.
    */
    PortableServer::POA_var m_objects_poa;
    
    /**
    * The factory name.
    */
    std::string m_factory_name;
    
}; // End class HelloUserFactory_impl 

}; // End namespace Tutorial




// ============================================================================
// Specialisation of template 
//    AnonymousObjectsFactoryAllocator<Cdmw::Tutorial::HelloUserFactory_impl> 
// for the method allocate
// ============================================================================
 
namespace LifeCycle
{

// Anonymous Objects Factory Allocator
template <>
PortableServer::Servant
AnonymousObjectsFactoryAllocator<Cdmw::Tutorial::HelloUserFactory_impl>::allocate (
                       CORBA::ORB_ptr orb, 
                       PortableServer::POA_ptr parent,
                       const std::string& factory_name,
                       Cdmw::OrbSupport::POAThreadingPolicy& threading_policy,
                       int & argc, char** argv)
    throw(Cdmw::OutOfMemoryException,
          Cdmw::BadParameterException,
          Cdmw::InternalErrorException,
          CORBA::SystemException)
{
    try 
    {
        return new Cdmw::Tutorial::HelloUserFactory_impl (orb,
                                                parent,
                                                factory_name,
                                                threading_policy);
    } 
    catch (const std::bad_alloc &) 
    {
        CDMW_THROW(Cdmw::OutOfMemoryException);
    } 
    catch (...) 
    {
        throw;
    }
}

}; // End namespace LifeCycle


namespace Tutorial
{

typedef Cdmw::LifeCycle::AnonymousObjectsFactoryAllocator<HelloUserFactory_impl> HelloUserFactory_impl_Allocator;

}; // End namespace Tutorial

}; // End namespace Cdmw


#endif // INCL_HELLO_FACTORY_IMPL_HPP 
