/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
* @brief
*
* @author Rafaella Panella <fmorciano@amsjv.it>
*/
#include <iostream>
#include <string>
#include "testlbcommon/CORBAManager.hpp"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

CORBAManager::CORBAManager()
{
    is_initialized_ = false;
}

///////////////////////////////////////////////////////////////////////////////

CORBAManager::~CORBAManager()
{
}


CORBA::ORB_var 
CORBAManager::get_orb()
{
    if( !is_initialized_ ) 
    {
        std::cerr<<"Orb is non initialized!"<<endl;
        throw CORBAManager::InvalidObject();
    }
    
    return orb_;
}

///////////////////////////////////////////////////////////////////////////////

void 
CORBAManager::init(CORBA::ORB_ptr orb )
{
    if(!is_initialized_)
        {
            orb_ =  CORBA::ORB::_duplicate(orb);
            is_initialized_=true;
        }
}

///////////////////////////////////////////////////////////////////////////////
CORBAManager* 
CORBAManager::instance()
{
    static CORBAManager singleton;
    return &singleton;
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
