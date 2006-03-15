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


#ifndef INCL_CCM_CIF_CONNECTIONDESCRIPTION_IMPL_HPP
#define INCL_CCM_CIF_CONNECTIONDESCRIPTION_IMPL_HPP


#include <CCMContainer/idllib/Components.skel.hpp>


namespace {


} // End anonymous space


namespace Cdmw {

namespace CCM {

namespace CIF {

//
// IDL:omg.org/Components/ConnectionDescription:1.0
//
class ConnectionDescription_impl : virtual public OBV_Components::ConnectionDescription,
                                   virtual public CORBA::DefaultValueRefCountBase
{
    ConnectionDescription_impl(const ConnectionDescription_impl&);
    void operator=(const ConnectionDescription_impl&);

public:

    ConnectionDescription_impl();
    ~ConnectionDescription_impl();

    virtual CORBA::ValueBase* _copy_value();
};

class ConnectionDescriptionFactory : virtual public CORBA::ValueFactoryBase
{
    virtual CORBA::ValueBase* create_for_unmarshal();

public:
    ConnectionDescriptionFactory();
    virtual ~ConnectionDescriptionFactory();
};

} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw

#endif // INCL_CCM_CIF_CONNECTIONDESCRIPTION_IMPL_HPP


