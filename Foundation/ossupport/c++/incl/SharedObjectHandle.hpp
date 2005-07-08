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


#ifndef INCL_SHAREDOBJECTHANDLE_OS_HPP
#define INCL_SHAREDOBJECTHANDLE_OS_HPP

namespace Cdmw
{

    namespace OsSupport
    {


        /**
        * Purpose:
        *<p> Value used by dl functions (dlopen, dlclose, dlsym and dlerror)
        *
        */


        template <class T>

        class SharedObjectHandle_T
        {

            public:
                typedef typename T::FunctionHandle FunctionHandle;
                typedef typename T::OsShlibHandle OsShlibHandle;


                virtual FunctionHandle dlsym(const char* symname) = 0;
                virtual void print(std::ostream& os) = 0;
                virtual int dlclose() = 0;
                virtual bool is_null() = 0;

            protected:
                SharedObjectHandle_T()
                {}

                ;

        };

    } // End namespace ossupport
} // End namespace Cdmw

#endif




