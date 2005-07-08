/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#ifndef INCL_ORBSUPPORT_EXCEPTION_STRINGS_HPP
#define INCL_ORBSUPPORT_EXCEPTION_STRINGS_HPP

namespace Cdmw
{

    namespace OrbSupport
    {

        class ExceptionStrings
        {

            public:
                static const char* BAD_INV_ORDER_reasons[];
                static const char* BAD_PARAM_reasons[];
                static const char* INTERNAL_reasons[];
                static const char* NO_IMPLEMENT_reasons[];
                static const char* NO_MEMORY_reasons[];
                static const char* NO_PERMISSION_reasons[];
                static const char* NO_RESOURCES_reasons[];
                static const char* BAD_OPERATION_reasons[];
                static const char* OBJECT_NOT_EXIST_reasons[];
                static const char* INV_OBJREF_reasons[];
                static const char* TRANSIENT_reasons[];
        }

        ; // End class ExceptionStrings

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_EXCEPTION_STRINGS_HPP

