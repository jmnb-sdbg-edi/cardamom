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


#include "testftccmcif/TestThread.hpp"
#include "testftccmcif/TestNavigationWithFT.hpp"


TestFtCcmCif::TestThread::TestThread(
        CORBA::ORB_ptr orb,
        FT::Location primaryLocation,
        FT::Location backupLocation,
        Components::Deployment::Container_ptr containerPrimary,
        Components::Deployment::Container_ptr containerBackup)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_primaryLocation(primaryLocation),
      m_backupLocation(backupLocation),
      m_containerPrimary(Components::Deployment::Container::_duplicate(containerPrimary)),
      m_containerBackup(Components::Deployment::Container::_duplicate(containerBackup))
{}


TestFtCcmCif::TestThread::~TestThread()
    throw()
{}


void TestFtCcmCif::TestThread::run() 
    throw()
{
    // Testing TestNavigation
    TestFtCcmCif::TestNavigationWithFT test_navi(m_orb.in(),
                                                 m_primaryLocation,
                                                 m_backupLocation,
                                                 m_containerPrimary.in(),
                                                 m_containerBackup.in());
    test_navi.start();

    m_orb->shutdown(false);
}   

