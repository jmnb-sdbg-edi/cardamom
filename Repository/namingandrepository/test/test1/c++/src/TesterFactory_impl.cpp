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


#include "test1/TesterFactory_impl.hpp"


TesterFactory_impl::TesterFactory_impl(const std::string& info)
: m_info(info)
{

}


CORBA::Boolean TesterFactory_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{

    bool res = false;

    if (!strcmp(repository_id, "IDL:Tester:1.0"))
        res = true;
	
	return res;

}


CdmwLifeCycle::RepositoryIdSeq* TesterFactory_impl::get_supported_objects()
    throw(CORBA::SystemException)
{

    CORBA::RepositoryId_var repId = CORBA::string_dup("IDL:Tester:1.0");
    
    CdmwLifeCycle::RepositoryIdSeq_var repIdSeq = new CdmwLifeCycle::RepositoryIdSeq();
    repIdSeq->length(1);
    repIdSeq->operator[](0) = repId;
    
    return repIdSeq._retn();

}


char* TesterFactory_impl::info()
    throw(CORBA::SystemException)
{

    return CORBA::string_dup(m_info.c_str());

}


