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


#ifndef INCL_TEST_FT_CCM_CIF_TESTTHREAD_HPP 
#define INCL_TEST_FT_CCM_CIF_TESTTHREAD_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <CCMContainer/idllib/Components.stub.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

namespace TestFtCcmCif
{


class TestThread : public Cdmw::OsSupport::Thread 
{
public:
    TestThread(CORBA::ORB_ptr orb,
               FT::Location primaryLocation,
               FT::Location backupLocation,
               Components::Deployment::Container_ptr containerPrimary,
               Components::Deployment::Container_ptr containerBackup)
        throw (CORBA::SystemException);

    virtual ~TestThread()
        throw ();
        
protected:
    void run() 
        throw();
    
private:
    
    TestThread();
    TestThread(const TestThread&);
    TestThread& operator = (const TestThread&);
    
    CORBA::ORB_var m_orb;
    FT::Location m_primaryLocation;
    FT::Location m_backupLocation;
    Components::Deployment::Container_var m_containerPrimary;
    Components::Deployment::Container_var m_containerBackup;
};


}; // End namespace 

#endif // INCL_TEST_FT_CCM_CIF_TESTTHREAD_HPP 

