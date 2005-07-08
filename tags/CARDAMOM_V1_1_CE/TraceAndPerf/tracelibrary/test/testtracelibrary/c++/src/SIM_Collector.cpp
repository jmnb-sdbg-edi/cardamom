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


#include "testtracelibrary/SIM_Collector.hpp"

#include <iostream>


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


SIM_Collector::SIM_Collector()
    throw()
{

    m_nbMessages = 0;
    m_the_messages = new CdmwTrace::FormattedMessageSeq;
    m_the_messages->length(0);

}


SIM_Collector::~SIM_Collector()
    throw()
{
}




void SIM_Collector::put_formatted_messages (  
                      const CdmwTrace::MessagesHeader& the_messages_header,
                      const CdmwTrace::FormattedMessageSeq& the_messages)
    throw(CORBA::SystemException)                       
{

    CORBA::ULong nbMessages = the_messages.length();

    // The header is simply replaced
    m_the_messages_header = new CdmwTrace::MessagesHeader(the_messages_header);

    // The messages are appended
    if (nbMessages > 0)
    {
      m_the_messages->length (m_nbMessages + nbMessages);

      for (CORBA::ULong i = 0 ; i < nbMessages ; i++)
      {
        m_the_messages[m_nbMessages+i] = the_messages[i];
      }

      m_nbMessages += nbMessages;
    }
}                      

void SIM_Collector::update (CdmwTrace::TraceProducer* p_trace_producer)
    throw(CORBA::SystemException)                       
{

}

CdmwTrace::TraceFilterSeq* SIM_Collector::get_trace_filters()
		    throw(CORBA::SystemException)
{
		  return NULL;
}


void SIM_Collector::activate_level (const char* a_domain,
                                    CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{

}



void SIM_Collector::deactivate_level (const char* a_domain,
                                      CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{

}

																			

const CdmwTrace::MessagesHeader& SIM_Collector::get_MessagesHeader()
{
    return m_the_messages_header.in();
}



const CdmwTrace::FormattedMessageSeq& SIM_Collector::get_FormattedMessageSeq()
{
    return m_the_messages.in();
}


}; // End namespace Trace
}; // End namespace Cdmw

