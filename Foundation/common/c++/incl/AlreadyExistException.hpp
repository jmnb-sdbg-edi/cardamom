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

#ifndef INCL_CDMW_COMMON_ALREADYEXISTEXCEPTION_HPP
#define INCL_CDMW_COMMON_ALREADYEXISTEXCEPTION_HPP

#include <string>

#include "Foundation/common/Exception.hpp"

namespace Cdmw {
namespace Common {

/**
*Purpose:
*<p>
*This exception indicates that an attempt to add something to a set or
*to create something has failed because there is already one of whatever
*it is and that is all that's permitted.
*/

class AlreadyExistException : public Exception
{
  public:
    AlreadyExistException(const std::string& = "AlreadyExistException");
};

} // End namespace Common
} // End namespace Cdmw

#endif /* INCL_CDMW_COMMON_ALREADYEXISTEXCEPTION_HPP */

