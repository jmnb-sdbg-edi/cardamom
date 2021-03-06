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

#ifndef INCL_CDMW_TEST_TESTER_FT_REPOSITORY_IMPL_HPP 
#define INCL_CDMW_TEST_TESTER_FT_REPOSITORY_IMPL_HPP 


#include "testftrepository/TesterFTRepository.skel.hpp"


namespace Cdmw
{

namespace Test
{

class TesterFTRepository_impl :
public virtual POA_Cdmw::Test::TesterFTRepository,
        virtual public PortableServer::RefCountServantBase
{

public:
    
    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    TesterFTRepository_impl(std::string name)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     * 
     */ 
    virtual 
    ~TesterFTRepository_impl()
        throw();
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    virtual char* name ()
        throw(CORBA::SystemException);
               
private:
    
    // Hide copy constructor/assignment operator
    TesterFTRepository_impl(const TesterFTRepository_impl& rhs)
        throw();
    
    TesterFTRepository_impl&
    operator=(const TesterFTRepository_impl& rhs)
        throw();
    
    std::string m_name;
    
    
}; // End class TesterFTRepository_impl 


}; // End namespace Test

}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_TESTER_FT_REPOSITORY_IMPL_HPP 

