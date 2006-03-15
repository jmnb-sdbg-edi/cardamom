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

#ifndef INCL_CDMW_TEST_STATE_MANAGER_HPP
#define INCL_CDMW_TEST_STATE_MANAGER_HPP

// include files
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "teststorageinterface/TestDataStore.stub.hpp"

// class definition

class TestStateManager : public Cdmw::TestUtils::Testable
{

public:
  TestStateManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA);
  
  virtual ~TestStateManager();

protected:
  virtual void do_tests();

private:
    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
};

class DataStoreDumper :  public std::unary_function<std::pair<int, std::string> , void>
{
public:
    DataStoreDumper(std::ostream & os) : m_os(os) { }
    void operator()(const std::pair<int, std::string>&elt);

private:
    std::ostream & m_os;
};

class ComplexeDataStoreDumper :  public std::unary_function<std::pair<CdmwFT::Test::TimeStamp_var, std::string> , void>
{
public:
    ComplexeDataStoreDumper(std::ostream & os) : m_os(os) { }
    void operator()(const std::pair<CdmwFT::Test::TimeStamp_var, std::string>&elt);

private:
    std::ostream & m_os;
};


#endif

