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


#ifndef INCL_COMMNSVCS_NAMING_INVALIDNAME_EXCEPTION_HPP
#define INCL_COMMNSVCS_NAMING_INVALIDNAME_EXCEPTION_HPP

#include "Foundation/common/Exception.hpp"
#include <string>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
namespace CommonSvcs
{
    
    /**
    * Namespace for CDMW Naming types and data.
    */

    namespace Naming
    {

        /**
        *Purpose:
        *<p>This exception will be used to indicate that a name is invalid.
        * It will be used when a CosNaming::Name is expected to contain
        * at least one component but it has not any.
        * This will also be used when an stringified name is syntactically malformed.
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class InvalidNameException : public Cdmw::Exception
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                InvalidNameException(const std::string& reason)
                throw();

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                InvalidNameException(const InvalidNameException& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~InvalidNameException()
                throw();

                /**
                *[Attribute description]
                */


            protected:


            private:
                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                InvalidNameException()
                throw();
                /**
                *[Attribute description]
                */ 
                //std::string m_reason;

        }; // End class InvalidNameException

    } // End namespace Naming
    
} // End namespace CommonSvcs
} // End namespace Cdmw

#endif // INCL_COMMNSVCS_NAMING_INVALIDNAME_EXCEPTION_HPP

