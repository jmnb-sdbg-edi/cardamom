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


#include "ccmcontainer/ValueTypeRegistration_impl.hpp"
#include "ccmcontainer/ValueTypeInstaller_impl.hpp"
#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include "ccmcontainer/MacroDefs.hpp"


namespace Cdmw
{

namespace CCM
{
namespace Container
{
    
ValueTypeRegistration_impl::ValueTypeRegistration_impl(CdmwCcmContainer::ValueTypeInstaller_ptr installer,
                                                       const std::string &                       repid)
    throw (CORBA::SystemException)
    : m_installer(CdmwCcmContainer::ValueTypeInstaller::_duplicate(installer)),
      m_repid(repid)
{
    DEBUG_ECHO << "ValueTypeRegistration_impl::ValueTypeRegistration_impl(" << repid << ")" << std::endl;
    
    CDMW_ASSERT(!CORBA::is_nil(installer));
    ValueTypeInstaller_impl* imp = dynamic_cast<ValueTypeInstaller_impl*>(installer);
    if (imp == 0)
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMWrongValuetypeInstallerType,
                               CORBA::COMPLETED_NO);
}

ValueTypeRegistration_impl::~ValueTypeRegistration_impl()
    throw ()
{
    DEBUG_ECHO << "ValueTypeRegistration_impl::~ValueTypeRegistration_impl(" << m_repid << ")" << std::endl;
    ValueTypeInstaller_impl* imp = dynamic_cast<ValueTypeInstaller_impl*>(m_installer.in());
    CDMW_ASSERT(imp != 0);
    imp->unregister_valuetype_factory(m_repid);
}

void
ValueTypeRegistration_impl::unregister()
    throw (CORBA::SystemException)
{
}

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw


