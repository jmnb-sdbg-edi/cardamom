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

#include "Time/clockservice/test/testcommon/PerformanceManager.hpp"

///////////////////////////////////////////////////////////////////////////////

PerformanceManager::PerformanceManager()
{
    is_active_ =  false;
}

///////////////////////////////////////////////////////////////////////////////

PerformanceManager::~PerformanceManager()
{
}

///////////////////////////////////////////////////////////////////////////////

PerformanceManager* PerformanceManager::get_instance()
{
    static  PerformanceManager  singleton;
    return &singleton;
}

///////////////////////////////////////////////////////////////////////////////

void    PerformanceManager::activate()
{
    is_active_ = true;
}

///////////////////////////////////////////////////////////////////////////////

void    PerformanceManager::de_activate()
{
    is_active_ = false;
}

///////////////////////////////////////////////////////////////////////////////

bool    PerformanceManager::is_active() const
{
    return is_active_;
}

///////////////////////////////////////////////////////////////////////////////

void    PerformanceManager::check_CDMW_ASSERTion(bool test,
                                            const CppUnit::SourceLine& sl)
{
    if( is_active_ && !test )
        throw PerformanceException(CppUnit::Message("Performance error"),sl);
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
