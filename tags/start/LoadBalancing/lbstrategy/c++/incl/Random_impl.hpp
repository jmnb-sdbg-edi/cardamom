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
#ifndef INCL_CDMW_LB_RANDOM_IMPL_H
#define INCL_CDMW_LB_RANDOM_IMPL_H

/**
 * @brief Random Strategy Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <LoadBalancing/idllib/CdmwLBService.skel.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>

#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>

#include <vector>

namespace Cdmw
{
    
namespace LB
{

    class Random_impl : public CdmwLB::Strategy
    {
    public:

        // Constructor.
        Random_impl();

        Random_impl(PortableGroup::ObjectGroup_ptr og,
                    Cdmw::LB::IOGRFactory* iogr_factory,
                    const char* name);
        
        // Destructor
        ~Random_impl();
        
        // IDL specific methods

        char* name()throw (CORBA::SystemException);

        /**
         * Return the next member to invoke, according to the Random Strategy
         *
         * @param ClientRequestInfo the request to invoke
         */
        
        virtual CORBA::Object_ptr next_member (PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException);
        
        
    private:
        // The next replica
        CORBA::ULong m_next;

        // Number of replicas
        CORBA::ULong m_replicaNumber;

        // Strategy Name
        CORBA::String_var m_name;

        // Object Group version
        PortableGroup::ObjectGroupRefVersion m_og_version;

        // The vector containing all replicas to invoke
        std::vector<CORBA::Object_ptr> m_og_members;

        // The IOGR Factory reference
        Cdmw::LB::IOGRFactory* m_iogr_factory;

        // The vector containing all fallback replicas
        std::vector<CORBA::Object_ptr> m_fallback_members;

        // Initialize all strategy attributes
        void init(PortableGroup::ObjectGroup_ptr og)
            throw (CORBA::SystemException);
    };
    
};//close namespace LB

};//close namespace Cdmw

#endif //INCL_CDMW_LB_RANDOM_IMPL_H
