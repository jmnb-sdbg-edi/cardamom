/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @brief Implementation for Client Request Interceptor.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#ifndef INCL_CDMW_LB_LBINIT_CLIENT_REQUEST_INTERCEPTOR_H
#define INCL_CDMW_LB_LBINIT_CLIENT_REQUEST_INTERCEPTOR_H

// Cdmw Files
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <LoadBalancing/idllib/CdmwLBService.stub.hpp>
#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>
#include <LoadBalancing/lbcommon/LBConfiguration.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <map>


namespace Cdmw
{
    
namespace LB
{
    
namespace LBInit
{

class ClientRequestInterceptor_impl : public PortableInterceptor::ClientRequestInterceptor,
                                      public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    /// Constructor.
    ClientRequestInterceptor_impl (CORBA::ORB_ptr orb,
                                   const char *name, 
                                   Cdmw::LB::IOGRFactory* iogr_factory);
    //
    // IDL to C++ Mapping
    //
    virtual void destroy ()
        throw(CORBA::SystemException);
    
    virtual char * name ()
        throw(CORBA::SystemException);
    
    virtual void send_request (
        PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException,
               PortableInterceptor::ForwardRequest);
    
    virtual void send_poll (
        PortableInterceptor::ClientRequestInfo_ptr ri)
        throw(CORBA::SystemException);
    
    virtual void receive_reply (
        PortableInterceptor::ClientRequestInfo_ptr ri)
        throw(CORBA::SystemException);
    
    virtual void receive_exception (
        PortableInterceptor::ClientRequestInfo_ptr ri)
        throw(CORBA::SystemException,
              PortableInterceptor::ForwardRequest);
    
    virtual void receive_other (
        PortableInterceptor::ClientRequestInfo_ptr ri)
      throw(CORBA::SystemException,
            PortableInterceptor::ForwardRequest);
    
private:
    // The orb reference
    CORBA::ORB_var m_orb;
    
    // The name of this interceptor.
    CORBA::String_var m_name;


    Cdmw::LB::IOGRFactory* m_iogr_factory;
    
    CdmwLB::StrategyFactory* m_lb_factory;

    int m_isIn;
  
  
    struct LBDataStructure
    {
	CdmwLB::Strategy_ptr  strategy_ptr ;
	IOP::ServiceContext   svc_cxt;
	PortableGroup::ObjectGroupRefVersion obj_group_version;
	int isLB;
    };

  
    typedef std::map <size_t , LBDataStructure *> LBCache;
    LBCache m_lb_cache;
    LBCache::iterator m_lb_cache_pos; 
};
    
};//End of namespace LBInit

};//End of namespace LB

};//End of namespace Cdmw

#endif  /* INCL_CDMW_LB_LBINIT_CLIENT_REQUEST_INTERCEPTOR_H */
