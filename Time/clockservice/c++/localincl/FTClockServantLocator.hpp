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

#ifndef CDMW_TIME_FT_SERVANT_LOCATOR_HPP
#define CDMW_TIME_FT_SERVANT_LOCATOR_HPP

// -- Cdmw Foundation Includes --
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

extern "C++" {
    namespace Cdmw {
        namespace clock {
            class FTClockActivationHandler;
        }
    }
}

namespace Cdmw {
    namespace clock {
        class FTClockServantLocator;
    }
}

/**
 *  @todo Update documentation.
 */
class Cdmw::clock::FTClockServantLocator
    : public  virtual PortableServer::ServantLocator,
      public virtual Cdmw::OrbSupport::RefCountLocalObject
{

public: 
    FTClockServantLocator(const std::string& oid,
                          Cdmw::clock::FTClockActivationHandler* activation_handler);
    
    // Destructor
    virtual ~FTClockServantLocator();

    /**
     * @todo Update documentation.
     */
    virtual PortableServer::Servant 
    preinvoke(const PortableServer::ObjectId& oid,
              PortableServer::POA_ptr poa,
              const char* operation,
              PortableServer::ServantLocator::Cookie& the_cookie)
        throw (CORBA::SystemException);
    
        
    /**
     * @todo Update documentation.
     */
    virtual void 
    postinvoke(const PortableServer::ObjectId& oid,
               PortableServer::POA_ptr poa,
               const char* operation,
               PortableServer::ServantLocator::Cookie the_cookie,
               PortableServer::Servant the_servant)
        throw (CORBA::SystemException);

private: 
    
    std::string                             m_oid_str;
    Cdmw::clock::FTClockActivationHandler*  m_activation_handler;
};

#endif /* CDMW_TIME_FT_SERVANT_LOCATOR_HPP */
