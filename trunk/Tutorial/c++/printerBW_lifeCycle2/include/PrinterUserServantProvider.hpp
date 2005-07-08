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

#ifndef INCL_PRINTER_USER_SERVANT_PROVIDER_HPP
#define INCL_PRINTER_USER_SERVANT_PROVIDER_HPP

#include <string>


#include <Foundation/osthreads/Mutex.hpp>
#include <LifeCycle/lifecycle/ServantProviderAllocator.hpp>

#include "PrinterServantProvider.hpp"

namespace Cdmw
{
namespace Tutorial
{


class PrinterUserServantProvider : public Cdmw::Tutorial::PrinterServantProvider
{
    
   friend class Cdmw::LifeCycle::ServantProviderAllocator<PrinterUserServantProvider>;
    
    
   public:

        virtual 
        ~PrinterUserServantProvider()
                throw();

        /**
        * Purpose:
        * <p> This method acts as a hand shaking between the Servant Provider
        * and the generated Factory implementation. It indicates whether
        * a Servant provided by this Servant Provider may incarnate multiple
        * objects (with ObjectIds provided by this Servant Provider)
        *
        *@return <B>true</B> if a servant may incarnate multiple objects, 
        * <B>false</B> otherwise.
        */
        virtual bool may_incarnate_multiple_objects() const throw()
        {
            return false;
        }

        
        
        
        
        /**
        * Purpose:
        * <p> This method creates the oid of the Printer object
        *
        *@return object id
        */
        virtual PortableServer::ObjectId* create_printer (const char* device)
            throw ();
                               
        /**
        * Purpose:
        * <p> This method creates the Printer servant associated to
        *     the oid
        *
        *@return servant object
        */    
        PortableServer::Servant lookup_servant (const PortableServer::ObjectId& oid)        
            throw(PortableServer::ForwardRequest,
                  CORBA::SystemException);




        void release_servant (const PortableServer::ObjectId& oid,
                              PortableServer::POA_ptr  poa,
                              PortableServer::Servant  servant) 
            throw(CORBA::SystemException);
            
            
        // returns the repository id of the object with oid        
        std::string get_supported_interface(const PortableServer::ObjectId& oid) const
            throw(CORBA::OBJECT_NOT_EXIST);
            
            
    protected:

        PrinterUserServantProvider (CORBA::ORB_ptr  orb,
                                    PortableServer::POA_ptr poa,
                                    const Cdmw::LifeCycle::ObjectDeactivator& deactivator,
                                    int & argc, char** argv)
            throw(CORBA::SystemException);


    private:
    
        PrinterUserServantProvider()
                throw();

        // Hide copy constructor/assignment operator
        PrinterUserServantProvider(const PrinterUserServantProvider& rhs)
                throw();
        
        PrinterUserServantProvider&
        operator=(const PrinterUserServantProvider& rhs)
                throw();

      
        /**
        *[Attribute description]
        */ 
        CORBA::ORB_var  m_orb;
        
        PortableServer::POA_var m_poa;
        
        const Cdmw::LifeCycle::ObjectDeactivator& m_object_deactivator;
        
        // oid creation counter
        Cdmw::OsSupport::Mutex m_count_mutex;
        unsigned long m_counter;

}; // End class PrinterUserServantProvider

}; // End namespace Tutorial




// ============================================================================
// Default template
//    ServantProviderAllocator<Cdmw::Printer::PrinterUserServantProvider> 
// is used for the methods allocate and dispose
// ============================================================================

namespace LifeCycle
{
    
typedef Cdmw::LifeCycle::ServantProviderAllocator<Cdmw::Tutorial::PrinterUserServantProvider> USPAllocator;


}; // End namespace LifeCycle

}; // End namespace Cdmw


#endif // INCL_PRINTER_USER_SERVANT_PROVIDER_HPP
