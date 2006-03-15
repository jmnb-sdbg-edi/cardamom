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

#ifndef CORBA_TEST_MANAGER_HPP
#define CORBA_TEST_MANAGER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/TestManager.hpp"
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>
#include <string>
#include <typeinfo>
#include <stdexcept>

namespace Cdmw
{
  namespace TestUtils
  {       
    /**
     * Class to send parameter to test case.
     */
    class CORBATestManager
    :   public TestManager
    {
    public:
          
      //! Exception classes
      class InvalidArgument 
        : public std::logic_error
      {
      public:
        InvalidArgument()
          :std::logic_error("Not valid argument!")
        {
        }
      };
      
      class InvalidIOR      
        : public std::logic_error
      {
      public:
        InvalidIOR()
          :std::logic_error("Not valid IOR!")
        {
        }
      };
      
      class InvalidObject   
        : public std::logic_error
      {
      public:
        InvalidObject()
          :std::logic_error("Not valid object!")
        {
        }
      };
      
      CORBATestManager();
      ~CORBATestManager();
      
      /**
      * POA activation is required if application is
      * only client.
      */
      void
      init(CORBA::ORB_ptr orb,
           PortableServer::POA_ptr  poa,
           int      argc,
           char***  argv );
     
      CORBA::Object_ptr 
      get_object( const std::string& name )
        throw (InvalidObject);
      
      CORBA::ORB_ptr
      get_ORB();
      
      void  
      add(const std::string& service_alias )
        throw (InvalidObject);
      
      static  CORBATestManager* 
      instance();
      
      
      PortableServer::POA_ptr
      get_POA();
      
    private:
     
      typedef std::map<std::string, CORBA::Object_ptr> db_type; 	
      typedef db_type::iterator db_iterator;  

      db_type m_db; 
        
      CORBA::ORB_ptr m_orb;
        	  
      PortableServer::POA_ptr m_rootPOA;
        
      static CORBATestManager* m_instance; 
	
      std::vector<std::string> m_test_list;      
    };    
  } // end of TestUtils namespace
} // end of Cdmw

#endif // CORBA_TEST_MANAGER_HPP
