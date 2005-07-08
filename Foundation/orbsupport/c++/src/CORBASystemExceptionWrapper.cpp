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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"


#define TEST_AND_WRAPP(T,sys_ex,reason) \
    (const T * ex__ = dynamic_cast<const T *>(sys_ex)) \
    do { \
        return new CORBASystemExceptionWrapperT<T>(*ex__,reason); \
    } while(0)


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

        CORBASystemExceptionWrapper*
        wrapp_system_exception(const CORBA::SystemException & e, std::string reason)
        {
            if TEST_AND_WRAPP(CORBA::TRANSIENT, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::BAD_PARAM, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::BAD_INV_ORDER, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::COMM_FAILURE, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INTERNAL, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::OBJECT_NOT_EXIST, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::NO_MEMORY, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::IMP_LIMIT, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INV_OBJREF, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::UNKNOWN, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::NO_PERMISSION, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::MARSHAL, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INITIALIZE, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::NO_IMPLEMENT, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::BAD_TYPECODE, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::BAD_OPERATION, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::NO_RESOURCES, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::NO_RESPONSE, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::PERSIST_STORE, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::FREE_MEM, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INV_IDENT, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INV_FLAG, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INTF_REPOS, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::BAD_CONTEXT, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::OBJ_ADAPTER, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::DATA_CONVERSION, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::TRANSACTION_REQUIRED, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::TRANSACTION_ROLLEDBACK, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INVALID_TRANSACTION, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::INV_POLICY, &e, reason)

                ;
            else if TEST_AND_WRAPP(CORBA::CODESET_INCOMPATIBLE, &e, reason)

                ;
            else // Not supported CORBA::SystemException
                return 0;
        };


    } // End namespace OrbSupport

} // End namespace Cdmw


