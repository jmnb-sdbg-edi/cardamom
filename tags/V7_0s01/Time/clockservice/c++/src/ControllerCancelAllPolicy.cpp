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

#include "clockservice/ControllerCancelAllPolicy.hpp"
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>

////////////////////////////////////////////////////////////////////////////////

namespace 
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerCancelAllPolicy::ControllerCancelAllPolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerCancelAllPolicy::~ControllerCancelAllPolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerCancelAllPolicy::on_set (
    ::Cdmw::clock::ControllerEx_ptr controller )
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->terminate();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerCancelAllPolicy::on_set_rate (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->terminate();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerCancelAllPolicy::on_pause (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->pause();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerCancelAllPolicy::on_terminate (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->terminate();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerCancelAllPolicy::on_resume(
	::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->resume();    
}

////////////////////////////////////////////////////////////////////////////////
// End Of File
////////////////////////////////////////////////////////////////////////////////
