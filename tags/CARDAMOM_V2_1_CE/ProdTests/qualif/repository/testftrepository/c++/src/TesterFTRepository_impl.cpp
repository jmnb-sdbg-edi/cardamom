/* =========================================================================== *
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
 * =========================================================================== */

// Cdmw Files

#include <sstream>
#include <fstream>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Assert.hpp"

#include <testftrepository/TesterFTRepository_impl.hpp>

namespace Cdmw
{
namespace Test
{


TesterFTRepository_impl::TesterFTRepository_impl(std::string name)
    throw(CORBA::SystemException)
    : m_name(name)
{ 

}


TesterFTRepository_impl::~TesterFTRepository_impl()
    throw()
{
}


char * TesterFTRepository_impl::name ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### "<<m_name<<" ######>" << std::endl;
    return CORBA::string_dup(m_name.c_str());
}



}; // End namespace Test

}; // End namespace Cdmw

