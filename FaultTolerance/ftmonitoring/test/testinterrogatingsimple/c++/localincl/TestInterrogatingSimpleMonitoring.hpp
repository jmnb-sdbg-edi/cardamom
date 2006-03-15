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

#ifndef __TestInterrogatingSimpleMonitoring_hpp__
#define __TestInterrogatingSimpleMonitoring_hpp__

// include files
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include "idllib/CdmwFTMonitoring.skel.hpp"

#include <Foundation/orbsupport/CORBA.hpp>
#include "ftmonitoring/common/CommonTypeDefinitions.hpp"
#include "Foundation/testutils/Testable.hpp"

using namespace CdmwFT;

class TestMonitoringObserver_impl : 
  public virtual MonitoringLibrary::MonitoringObserver,
  public virtual Cdmw::OrbSupport::RefCountLocalObject
{
  public:
  TestMonitoringObserver_impl() {}
  virtual ~TestMonitoringObserver_impl() {}

  virtual void on_start (const char * the_location)
    throw(CORBA::SystemException);

  virtual void on_failure (const char * the_location)
    throw(CORBA::SystemException);
};

// class definition

class TestInterrogatingSimpleMonitoring : public Cdmw::TestUtils::Testable
{

public:
  TestInterrogatingSimpleMonitoring();
  
  TestInterrogatingSimpleMonitoring(int argc, char* argv[], int timeOn);

  virtual ~TestInterrogatingSimpleMonitoring();

protected:
  virtual void do_tests();

private:

  int m_timeOn;
  CORBA::ORB_var m_orb;  
  Cdmw::FT::Supervision::ArgvList m_argList;
};

#endif

