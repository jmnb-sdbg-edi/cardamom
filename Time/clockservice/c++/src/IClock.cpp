/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/IClock.hpp>

using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

IClock::~IClock()
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

::TimeBase::TimeT 
IClock::virtual_to_real(const ::TimeBase::TimeT)  const
{
    return const_cast<IClock*>(this)->current_time();
}

///////////////////////////////////////////////////////////////////////////////

double  
IClock::retrieve_rate() const
{
    return 1.0;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

