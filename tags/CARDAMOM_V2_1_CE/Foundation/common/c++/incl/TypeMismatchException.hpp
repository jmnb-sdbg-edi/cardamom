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


#ifndef INCL_COMMON_TYPEMISMATCH_EXCEPTION_HPP
#define INCL_COMMON_TYPEMISMATCH_EXCEPTION_HPP

#include <Foundation/common/Exception.hpp>
#include <string>

namespace Cdmw
{

    namespace Common
    {

        /**
        *Purpose:
        *<p>This exception will be used to indicate that a CORBA narrow operation 
        * didn't succeed because the two types are not compatible.
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class TypeMismatchException : public Cdmw::Exception
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TypeMismatchException()
                throw();

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TypeMismatchException(const std::string& reason)
                throw();

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TypeMismatchException(const TypeMismatchException& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~TypeMismatchException()
                throw();

                /**
                *[Attribute description]
                */


            protected:


            private:
                /**
                *[Attribute description]
                */

        }

        ; // End class TypeMismatchException

    } // End namespace Common
} // End namespace Cdmw

#endif // INCL_CDMW_TYPEMISMATCH_EXCEPTION_HPP

