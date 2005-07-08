/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_CDMW_CCM_CONTAINER_COMPONENT_SERVANT_PROVIDER_HPP
#define INCL_CDMW_CCM_CONTAINER_COMPONENT_SERVANT_PROVIDER_HPP

#include <LifeCycle/lifecycle/ServantProviderBase.hpp>
#include "CCMContainer/idllib/CdmwCcmContainer.stub.hpp"

namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
    * Purpose:
    * <p> A special servant provider class for component and facet servants.
    *
    */
    class ComponentServantProvider : public Cdmw::LifeCycle::ServantProviderBase
    {
    public:
        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~ComponentServantProvider()
            throw() {}
        
        /**
        * Purpose:
        * <p>Operation to be called by the container in order to set the component context.
        *
        *@param ctxt A reference to a CCM2Context object
        *@exception TODO:
        */
        virtual
        void set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
            throw (Components::CCMException,
                   CORBA::SystemException) = 0;
        
    protected:
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        ComponentServantProvider() throw()
            : Cdmw::LifeCycle::ServantProviderBase()
        {}
    private:

        // Hide copy constructor/assignment operator
        ComponentServantProvider(const ComponentServantProvider& rhs)
                throw();        
        ComponentServantProvider&
        operator=(const ComponentServantProvider& rhs)
                throw();
    };

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_COMPONENT_SERVANT_PROVIDER_HPP

