/* =========================================================================== *
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
 * =========================================================================== */

#include "Foundation/orbsupport/CORBA.hpp"
#include "eventdata/DataB_impl.hpp"

CORBA::ValueBase* DataBFactory::create_for_unmarshal()
{
    DataB_var data = new DataB_impl();
    return data._retn();
}

DataB* DataBFactory::create( const char* message, CORBA::ULong number )
{
    DataB_var data = new DataB_impl( message, number );
    return data._retn();
}

DataB_impl::DataB_impl()
: OBV_DataB(), DataA_impl()
{
    data_type( TYPE_B );
}

DataB_impl::DataB_impl( const char* theMessage, CORBA::ULong theNumber )
: OBV_DataB(), DataA_impl( theMessage )
{
    data_type( TYPE_B );
    number( theNumber );
}

CORBA::ValueBase* DataB_impl::_copy_value()
{
    DataB_var copy = new DataB_impl( message(), number() );
    return copy._retn();
}

