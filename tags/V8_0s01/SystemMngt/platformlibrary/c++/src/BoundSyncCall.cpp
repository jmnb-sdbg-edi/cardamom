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


#include "Foundation/common/Assert.hpp"
#include "platformlibrary/OrbacusBoundSyncCall.hpp"
#include "platformlibrary/TAOBoundSyncCall.hpp"
#include <string>

namespace Cdmw
{
namespace PlatformMngt
{

BoundSyncCallFactory* BoundSyncCallFactory::M_factory = NULL;
CORBA::ORB_ptr BoundSyncCallFactory::M_orb = CORBA::ORB::_nil();


// ----------------------------------------------------------------------
// BoundSyncCallFactory class.
// ----------------------------------------------------------------------
BoundSyncCallFactory::BoundSyncCallFactory()
{
}

BoundSyncCallFactory::~BoundSyncCallFactory()
{
}

void
BoundSyncCallFactory::initialise( CORBA::ORB_ptr orb )
throw( BadParameterException )
{
    if( CORBA::is_nil( orb ) )
        CDMW_THROW2( BadParameterException, "orb", "is nil" );

    M_orb = CORBA::ORB::_duplicate( orb );
}

void
BoundSyncCallFactory::destroy()
{
    if( M_factory != NULL )
    {
        delete M_factory;
        M_factory = NULL;
    }

    if( !CORBA::is_nil( M_orb ) )
        CORBA::release( M_orb );
}

BoundSyncCallFactory*
BoundSyncCallFactory::getFactory()
throw(OutOfMemoryException)
{
    CDMW_ASSERT(!CORBA::is_nil(M_orb));

    if( M_factory == NULL )
    {
        try
        {
            #if CDMW_ORB_VDR == orbacus
            M_factory = new ORBACUSBoundSyncCallFactory( M_orb );
            #elif CDMW_ORB_VDR == tao
            M_factory = new TAOBoundSyncCallFactory( M_orb );
            #endif
        }
        catch( const std::bad_alloc& )
        {
            CDMW_THROW( OutOfMemoryException );
        }
    }
    
    return M_factory;
}

// ----------------------------------------------------------------------
// BoundSyncCall class.
// ----------------------------------------------------------------------

const char* UNKNOWN_REP_ID = "- NO MEMORY : REPOSITORY ID NOT SET -";
const char* UNKNOWN_NAME = "- NO MEMORY : NAME NOT SET -";

BoundSyncCall::~BoundSyncCall()
{
}

const char*
BoundSyncCall::getExceptionRepId() throw()
{
    if (m_exceptionRepId.empty())
        return UNKNOWN_REP_ID;
    else
        return m_exceptionRepId.c_str();
}

const char*
BoundSyncCall::getExceptionName() throw()
{
    if (m_exceptionName.empty())
        return UNKNOWN_NAME;
    else
        return m_exceptionName.c_str();
}

void
BoundSyncCall::setExceptionInfo( const CORBA::Exception& exception )
    throw()
{
    try
    {
        m_exceptionRepId = exception._rep_id();
        m_exceptionName = exception._name();
    }
    catch(const std::bad_alloc &)
    {
        m_exceptionRepId.erase();
        m_exceptionName.erase();
    }

}

} // End namespace PlatformMngt
} // End namespace Cdmw

