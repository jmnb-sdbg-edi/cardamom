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


#ifndef INCL_NAMINGCONTEXTBINDER_HPP
#define INCL_NAMINGCONTEXTBINDER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "Foundation/testutils/Testable.hpp"

class NamingContextBinder : virtual public Cdmw::TestUtils::Testable
{

protected:
    PortableServer::POA_ptr m_rootPOA;
    CORBA::ORB_ptr m_ORB;


public:

    NamingContextBinder(const std::string& name,
        PortableServer::POA_ptr rootPOA,
        CORBA::ORB_ptr orb);

    virtual ~NamingContextBinder();

protected:

	virtual void do_tests();

    void display_binding_list(const CosNaming::BindingList& bl);
    void display_binding(const CosNaming::Binding& b);

};

#endif // INCL_NAMINGCONTEXTBINDER_HPP

