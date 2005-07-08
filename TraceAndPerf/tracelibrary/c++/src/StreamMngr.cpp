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


#include "TraceAndPerf/tracelibrary/StreamMngr.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/


//
// Ctor
//
StreamMngr::StreamMngr()
    throw()
{

    // Nothing to do

}




//
// Dtor
//
StreamMngr::~StreamMngr()
    throw()
{
    // Nothing to do
}





//
// Return the singleton 
//
StreamMngr& StreamMngr::instance()
    throw()
{
    static StreamMngr singleton;

    return singleton;
}





//
// Register the thread for using get_stream()
//
void StreamMngr::register_thread()
    throw (OutOfMemoryException,
           AssertionFailedException)
{
    try 
	{
        CDMW_MUTEX_GUARD (m_streamMap_mtx);

        StreamMap::iterator itr = m_streamMap.find (OsSupport::Thread::self());

        // The thread is already registered
        CDMW_ASSERT (itr == m_streamMap.end());

        m_streamMap[OsSupport::Thread::self()] = new Stream();

    } 
	catch (const std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
}    



//
// Unregister the thread for using get_stream()
//
void StreamMngr::unregister_thread()
    throw( AssertionFailedException )
{
    CDMW_MUTEX_GUARD (m_streamMap_mtx);
    
    StreamMap::iterator itr = m_streamMap.find (OsSupport::Thread::self());

    // The thread must have call register_thread before calling this function
    CDMW_ASSERT (itr != m_streamMap.end());

    // We destroy the Stream
    delete itr->second;
    m_streamMap.erase (itr);
}


Stream& StreamMngr::get_stream()
{
    CDMW_MUTEX_GUARD (m_streamMap_mtx);

    StreamMap::iterator itr = m_streamMap.find (OsSupport::Thread::self());

    // The thread must have call register_thread before calling this function
    CDMW_ASSERT (itr != m_streamMap.end());

    // The map contains a Stream*, and we want return a Stream&, so *(*itr)
    return *(itr->second);
}


} // End namespace Trace
} // End namespace Cdmw


