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


#ifndef INCL_MIOP_TESTER_IMPL_HPP 
#define INCL_MIOP_TESTER_IMPL_HPP 


#include <testmiop/MIOPTester.skel.hpp>
#include <map>


namespace CdmwFT
{

namespace Test
{

    
    
class MIOPTester_impl :
    public virtual POA_CdmwFT::Test::MIOPTester,
    public virtual PortableServer::RefCountServantBase
{

    public:

        MIOPTester_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);

        
        /**
         */
        virtual void test_miop_request (CORBA::UShort client_id,
                                        CORBA::UShort counter)
            throw(CORBA::SystemException);
                  


    private:

        // Hide copy constructor/assignment operator
        MIOPTester_impl(const MIOPTester_impl& rhs)
                throw();
        
        MIOPTester_impl&
        operator=(const MIOPTester_impl& rhs)
                throw();

        CORBA::ORB_var m_orb;
        
    public:
        std::map< CORBA::UShort, std::map< CORBA::UShort, bool > > m_received_requests;
        

}; // End class MIOPTester_impl 


// mutex to protect m_received_requests writing
static Cdmw::OsSupport::Mutex M_map_mutex;


}; // End namespace Test

}; // End namespace CdmwFT

#endif // INCL_MIOP_TESTER_IMPL_HPP 

