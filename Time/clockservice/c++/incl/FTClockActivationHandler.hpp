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

#ifndef CARDAMOM_FT_CLOCK_ACTIVATION_HANDLER_HPP 
#define CARDAMOM_FT_CLOCK_ACTIVATION_HANDLER_HPP 

// -- Cdmw Foundation Includes --
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

// -- Cdmw FaultTolerance Includes --
#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>

#include <Time/clockservice/FTClock_impl.hpp>

namespace Cdmw {
    namespace clock {
        class FTClockActivationHandler;
    }
}

/**
 * This class takes care of managing the activation and deactivation
 * of the <code>FTClock_impl</code> servant.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */
class Cdmw::clock::FTClockActivationHandler
    : virtual public CdmwFT::Location::ActivationHandler,
      virtual public Cdmw::OrbSupport::RefCountLocalObject
{
public:
    FTClockActivationHandler(const std::string&        name,
                             Cdmw::clock::FTClock_impl* executorImpl);
    
    virtual ~FTClockActivationHandler();

public:    
    virtual void activate()
        throw (CORBA::SystemException);
    
    virtual void deactivate()
        throw (CORBA::SystemException);
    
public:
    virtual bool is_active(); 
    virtual Cdmw::clock::FTClock_impl* getServant();
    
        
private: 
    std::string m_name;
    bool        m_is_active;
    
    Cdmw::clock::FTClock_impl* m_clock_impl;
};


#endif /* CARDAMOM_FT_CLOCK_ACTIVATION_HANDLER_HPP  */
