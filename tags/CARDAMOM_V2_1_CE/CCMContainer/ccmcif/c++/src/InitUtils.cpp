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


#include <CCMContainer/ccmcif/InitUtils.hpp>
#include <Foundation/common/Exception.hpp>

#include <CCMContainer/ccmcommon/Cookie_impl.hpp>
#include <CCMContainer/ccmcif/ComponentPortDescription_impl.hpp>
#include <CCMContainer/ccmcif/PortDescription_impl.hpp>
#include <CCMContainer/ccmcif/FacetDescription_impl.hpp>
#include <CCMContainer/ccmcif/ReceptacleDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConnectionDescription_impl.hpp>
#include <CCMContainer/ccmcif/SubscriberDescription_impl.hpp>
#include <CCMContainer/ccmcif/EmitterDescription_impl.hpp>
#include <CCMContainer/ccmcif/PublisherDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConsumerDescription_impl.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {


void 
InitUtils::register_valuetype_factories(CORBA::ORB_ptr orb)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var vf;
#else
    CORBA::ValueFactoryBase_var vf;
#endif

    try {
        vf = new FacetDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/FacetDescription:1.0", vf.in());
        
        vf = new PortDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/PortDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::Common::CookieFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/Cookie:1.0", vf.in());
        
        vf = new ConnectionDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/ConnectionDescription:1.0", vf.in());
        
        vf = new ReceptacleDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/ReceptacleDescription:1.0", vf.in());
        
        vf = new ConsumerDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/ConsumerDescription:1.0", vf.in());
        
        vf = new EmitterDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/EmitterDescription:1.0", vf.in());
        
        vf = new SubscriberDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/SubscriberDescription:1.0", vf.in());
        
        vf = new PublisherDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/PublisherDescription:1.0", vf.in());
        
        vf = new ComponentPortDescriptionFactory();
        vf = orb->register_value_factory ("IDL:omg.org/Components/ComponentPortDescription:1.0", vf.in());
    } catch (const CORBA::SystemException & ex ) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
    }
}


} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw

