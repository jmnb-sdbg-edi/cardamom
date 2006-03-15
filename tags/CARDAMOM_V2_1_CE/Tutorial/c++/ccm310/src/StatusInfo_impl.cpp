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


#include "StatusInfo_impl.hpp"

// Enty points

extern "C" 
{
    
    CORBA::ValueFactoryBase* createStatusInfoFactory () 
    {
        return new Hello::StatusInfoFactory();
    }
};


namespace Hello
{
    
// Constructor
StatusInfo_impl::StatusInfo_impl ()
{
}

// Copy Constructor
StatusInfo_impl::StatusInfo_impl (const StatusInfo_impl& right)
{
    text (right.text());
}

// Destructor
StatusInfo_impl::~StatusInfo_impl ()
{
}


CORBA::ValueBase* StatusInfo_impl::_copy_value()
{
    StatusInfo_impl* result = new StatusInfo_impl();
    result->text (text());

    return result;
}




/******************************/
/*         Factory            */
/******************************/

StatusInfoFactory::StatusInfoFactory()
{
}
    
StatusInfoFactory::~StatusInfoFactory()
{
}

CORBA::ValueBase* StatusInfoFactory::create_for_unmarshal ()
{
    return new StatusInfo_impl();
}


}; // End of namespace Hello
