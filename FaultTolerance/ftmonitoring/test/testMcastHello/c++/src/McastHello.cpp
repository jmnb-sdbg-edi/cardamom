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
//
// McastHello.cpp,v 1.3 2002/01/29 20:20:55 okellogg Exp
//
#include "testMcastHello/McastHello.h"

ACE_RCSID(McastHello, McastHello, "McastHello.cpp,v 1.3 2002/01/29 20:20:55 okellogg Exp")

McastHello::McastHello (CORBA::ORB_ptr orb,
                        int instance,
                        CORBA::Boolean &status)
  : orb_ (CORBA::ORB::_duplicate (orb)),
    instance_ (instance),
    status_ (status)
{
  // Initialize to false (failure)
  this->status_ = 0;
}

void
McastHello::send_forty_two (CORBA::Long forty_two ACE_ENV_ARG_DECL_NOT_USED)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  if (this->status_ == 0 &&
      forty_two == 42)
    {
      this->status_ = 1;
    }
  else
    this->status_ = 0;
}

void
McastHello::shutdown (ACE_ENV_SINGLE_ARG_DECL)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0 ACE_ENV_ARG_PARAMETER);
}
