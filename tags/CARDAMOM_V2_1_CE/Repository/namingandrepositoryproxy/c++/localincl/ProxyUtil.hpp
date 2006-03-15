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


#ifndef INCL_NAMINGANDREPOSITORY_PROXY_UTIL_HPP
#define INCL_NAMINGANDREPOSITORY_PROXY_UTIL_HPP

#include "Foundation/orbsupport/ObjectTraits.hpp"
#include <map>

namespace Cdmw
{
namespace NamingAndRepository
{

class ProxyUtil
{
protected:
    ProxyUtil(CORBA::ORB_ptr orb, PortableServer::POA_ptr parent_poa,
              const char *name);

    virtual ~ProxyUtil();

    /**
    * Purpose:
    * <p>
    * Redefines the PortableServer::ServantBase method
    * to return the correct POA.
    *
    * @return The m_poa member
    */
    virtual PortableServer::POA_ptr _default_POA();

    CORBA::ULong get_id() throw (CORBA::SystemException);

    CORBA::ULong next_id();

    PortableServer::POA_var m_poa;

private:
    PortableServer::Current_var m_current;

    CORBA::ULong m_next_id;
};

} // End of namespace NamingAndRepository
} // End of namespace Cdmw


// TODO: Add max retry counter.

#define CATCH_RECONSTRUCT(EXCEPTION)                    \
        catch (const EXCEPTION& ex)                     \
        {                                               \
            if (ex.completed() == CORBA::COMPLETED_NO)  \
            {                                           \
                reconstruct();                          \
            }                                           \
            else                                        \
            {                                           \
                throw;                                  \
            }                                           \
        }


#define RETRY(OP)                               \
{                                               \
    bool fail = 1;                              \
    do                                          \
    {                                           \
        try                                     \
        {                                       \
            get_object()->OP;                   \
            fail = 0;                           \
        }                                       \
        CATCH_RECONSTRUCT(CORBA::COMM_FAILURE)  \
        CATCH_RECONSTRUCT(CORBA::TRANSIENT)     \
        CATCH_RECONSTRUCT(CORBA::NO_RESPONSE)   \
        CATCH_RECONSTRUCT(CORBA::OBJ_ADAPTER)   \
    } while(fail);                              \
}

#define RETRY_RETURN(OP)                        \
{                                               \
    do                                          \
    {                                           \
        try                                     \
        {                                       \
            return get_object()->OP;            \
        }                                       \
        CATCH_RECONSTRUCT(CORBA::COMM_FAILURE)  \
        CATCH_RECONSTRUCT(CORBA::TRANSIENT)     \
        CATCH_RECONSTRUCT(CORBA::NO_RESPONSE)   \
        CATCH_RECONSTRUCT(CORBA::OBJ_ADAPTER)   \
    } while(1);                                 \
                                                \
    throw CORBA::INTERNAL();                    \
}

#define RETRY_SET_VALUE(OP, VAL)                \
{                                               \
    bool fail = 1;                              \
    do                                          \
    {                                           \
        try                                     \
        {                                       \
            VAL = get_object()->OP;             \
            fail = 0;                           \
        }                                       \
        CATCH_RECONSTRUCT(CORBA::COMM_FAILURE)  \
        CATCH_RECONSTRUCT(CORBA::TRANSIENT)     \
        CATCH_RECONSTRUCT(CORBA::NO_RESPONSE)   \
        CATCH_RECONSTRUCT(CORBA::OBJ_ADAPTER)   \
    } while(fail);                              \
                                                \
    throw CORBA::INTERNAL();                    \
}

#endif
