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


#include "testccmcontainer/Messages_impl.hpp"

// Enty points

extern "C" {
    
    CORBA::ValueFactoryBase* createMessageFactory () {
        return new TestCdmwCCM::MessageFactory();
    }
    
    CORBA::ValueFactoryBase* createTraceMessageFactory () {
        return new TestCdmwCCM::TraceMessageFactory();
    }
};

namespace TestCdmwCCM
{

    
Message_impl::Message_impl()
{
    the_message((const char*)"");
}
    
    
Message_impl::Message_impl(const char*       msg)
{
    the_message(msg);
}

CORBA::ValueBase*
Message_impl::_copy_value()
{
    return new Message_impl(the_message());
}

Message_impl::~Message_impl()
    throw ()
{
}

CORBA::ValueBase * 
MessageFactory::create_for_unmarshal ()
{
    return new Message_impl();
}



    
TraceMessage_impl::TraceMessage_impl()
{
    the_message((const char*)"");
    the_level(0L);
}
    
    
TraceMessage_impl::TraceMessage_impl(const char* msg, CORBA::Long level)
    : OBV_HelloModule::TraceMessage()
{
    the_message(msg);
    the_level(level);
}

CORBA::ValueBase*
TraceMessage_impl::_copy_value()
{
    return new TraceMessage_impl(the_message(), the_level());
}

TraceMessage_impl::~TraceMessage_impl()
    throw ()
{
}

CORBA::ValueBase * 
TraceMessageFactory::create_for_unmarshal ()
{
    return new TraceMessage_impl();
}



}; // End namespace TestCdmwCCM


