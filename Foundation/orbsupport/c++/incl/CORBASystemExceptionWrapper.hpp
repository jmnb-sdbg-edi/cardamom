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


#ifndef INCL_ORBSUPPORT_CORBA_SYSTEM_EXCEPTION_WRAPPER_HPP
#define INCL_ORBSUPPORT_CORBA_SYSTEM_EXCEPTION_WRAPPER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include <string>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{

    /**
    * Namespace for CDMW OrbSupport types and data.
    */

    namespace OrbSupport
    {
        //

        class CORBASystemExceptionWrapper : public Exception
        {

            public:

                CORBASystemExceptionWrapper(const CORBA::SystemException & ex,
                                            std::string reason)
                        : Exception( Exception::SAFE,
                                     OrbSupport::exception_to_string(ex)
                                     + " \n" + reason )
                {}

                ~CORBASystemExceptionWrapper()
                throw()
                {}


                virtual void raise_SystemException() const throw (CORBA::SystemException) = 0;

        }

        ; // end of CORBASystemExceptionWrapper


        //

        template <typename T>

        class CORBASystemExceptionWrapperT : public CORBASystemExceptionWrapper
        {

            public:
                CORBASystemExceptionWrapperT (const T & ex,
                                              std::string reason = std::string(""))
                        : CORBASystemExceptionWrapper(ex, reason),
                        m_system_exception(ex)
                {}

                CORBASystemExceptionWrapperT (const CORBASystemExceptionWrapperT<T> & e)
                        : CORBASystemExceptionWrapper(e),
                        m_system_exception(e.m_system_exception)
                {}

                ~CORBASystemExceptionWrapperT()
                throw()
                {}

                virtual void raise_SystemException() const throw (CORBA::SystemException)
                {
                    throw T(m_system_exception);
                }

            private:
                T m_system_exception;
        };

        CORBASystemExceptionWrapper*
        wrapp_system_exception(const CORBA::SystemException & e, std::string reason);

        //
        template <>

        class CORBASystemExceptionWrapperT<CORBA::SystemException> : public CORBASystemExceptionWrapper
        {

            public:

                CORBASystemExceptionWrapperT(const CORBA::SystemException & ex,
                                             std::string reason = std::string(""))
                        : CORBASystemExceptionWrapper(ex, reason),
                        m_wrapper(wrapp_system_exception(ex, reason))
                {
                }

                ~CORBASystemExceptionWrapperT()
                throw()
                {
                    if (m_wrapper)
                        delete m_wrapper;
                }


                virtual void raise_SystemException() const throw (CORBA::SystemException)
                {
                    m_wrapper->raise_SystemException();
                }

            private:
                CORBASystemExceptionWrapper * m_wrapper;
        }

        ; // end of CORBASystemExceptionWrapper

    } // End namespace OrbSupport

} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_CORBA_SYSTEM_EXCEPTION_WRAPPER_HPP

