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

#ifndef  CORBA_EXCEPTIONS_OSTREAM_HPP
#define  CORBA_EXCEPTIONS_OSTREAM_HPP

/**
 * @file
 * @brief  Wrapper for ostream of CORBA::Exception in WIN32.
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 */

#ifdef WIN32
#include <tao/corba.h>
#include <ostream>

/**
 * CORBA::Exception;
 * CORBA::UserException;
 * CORBA::SystemException;
 * CORBA::UnknownUserException;
 * CORBA::ExceptionList;
 * CORBA::ExceptionList_var;
 */

inline  std::ostream& 
operator<<(std::ostream& os,
           const CORBA::SystemException& e )
{
    return os<<e._name();
}

#endif

#endif /* CORBA_EXCEPTIONS_OSTREAM_HPP */
