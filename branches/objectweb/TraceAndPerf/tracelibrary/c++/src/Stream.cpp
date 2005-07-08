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


#include "TraceAndPerf/tracelibrary/Stream.hpp"
#include "TraceAndPerf/tracelibrary/Message.hpp"
#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: none 
*<p>
*Portability issues:  none 
*<p>
*/


//
// Ctor
//
Stream::Stream()
    throw()
{
    // Nothing to do
}




//
// Dtor
//
Stream::~Stream()
    throw()
{
    // Nothing to do
}




//
// Set the line origin of the trace
//
void Stream::set_line (size_t line)
	throw()
{
    m_line = line;	
}	




//
// Set the file origin of the trace
//
void Stream::set_file (const std::string& file)
	throw (OutOfMemoryException)
{
    try 
	{
        m_file = file;
	} 
	catch (std::bad_alloc&)
	{
	    CDMW_THROW (OutOfMemoryException);
	}
}




//
// Set the user level origin of the trace
//
void Stream::set_userLevel (int level)
	throw()
{
    m_userLevel = level;
}




//
// Set the user domain origin of the trace
//
void Stream::set_userDomain (const std::string& domain)
	throw (OutOfMemoryException)
{
    try 
	{
        m_userDomain = domain;

    } 
	catch (std::bad_alloc& ) 
	{
        CDMW_THROW (OutOfMemoryException);
    }
}



//
// Copy the data into the FlushAreaMngr and reset its content
//
void Stream::do_flush()
	throw (OutOfMemoryException)
{
    try
    {
        Message::MessageHeader header;

        header.m_fileLine = this->m_line;
        header.m_fileName = this->m_file;
        header.m_userLevel = this->m_userLevel;
        header.m_userDomain = this->m_userDomain;
        header.m_tid = OsSupport::Thread::self();
        header.m_timestamping = OsSupport::OS::get_time();

        FlushAreaMngr::instance().add_message (header, this->str());

        // We can reset the stream object
        this->str("");
    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}


} // End namespace Trace
} // End namespace Cdmw

