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


#ifndef INCL_PLATFORMMNGT_EVENT_IMPL_HPP
#define INCL_PLATFORMMNGT_EVENT_IMPL_HPP

#include <string>
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "idllib/CdmwPlatformMngtEvent.stub.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt


        /**
         *Purpose:
         *<p> The implementation of the Event value type.
         */

        class Event_impl : public virtual OBV_CdmwPlatformMngt::Event,
                    public virtual CORBA::DefaultValueRefCountBase
        {

            protected:

                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                Event_impl();

                /**
                * Returns the header string as following :
                * "YYYY/MM/DD HH:MM:SS LEV"
                */
                std::string headerString();

                /**
                * Returns the beginning part of the description string containing
                * the header, the issuer and the system name.
                */
                std::string beginString();

            public:


                /**
                * Purpose:
                * <p>
                * implements the
                * IDL:thalesgroup.com/CdmwPlatformMngt/Event/get_issuer:1.0
                * operation
                *
                * Returns the default issuer PFMMNG.
                */
                virtual char* get_issuer();

                /**
                 * Inherited from CORBA::ValueBase
                 */
                virtual CORBA::ValueBase* _copy_value() = 0;

        };

    } // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_EVENT_IMPL_HPP

