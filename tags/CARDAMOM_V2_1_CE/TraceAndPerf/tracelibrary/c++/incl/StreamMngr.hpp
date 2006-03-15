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


#ifndef INCL_STREAMMNGR_HPP 
#define INCL_STREAMMNGR_HPP 

#include "Foundation/common/System.hpp"

#include <sstream>
#include <string>
#include <map>

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "TraceAndPerf/tracelibrary/Stream.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Trace types and data.
*/
namespace Trace
{

// To suppress GCC warning about no Ctor/Dtor: we are a singleton
class foo;

/**
*Purpose:
*<p>    This class is in charge of providing a Stream to build
*       message at each thread requesting one. Before any request
*       the thread must register itself into the StreamMngr. This class
*       is a singleton.
*
*Features:
*<p> Thread safe
*
*@see  Stream 
*
*/
class StreamMngr
{

    public:

        /**
        * Purpose:
        * <p> Destructor
        */
        virtual 
        ~StreamMngr()
                throw();



        /**
        * Purpose:
        * <p> Returns the StreamMngr singleton.
        */
        static
        StreamMngr& instance()
            throw();



        
        /**
        * Purpose:
        * <p> Returns the Stream specific to the caller thread
        */
        Stream& get_stream();




        /**
        * Purpose:
        * <p> Register the thread, this method must be called 
        *  before any call to get_stream().
        *
        * @exception OutOfMemoryException
        * @exception AssertionFailedException        
        */
        void register_thread()
            throw (OutOfMemoryException,
                   AssertionFailedException);


        

        /**
        * Purpose:
        * <p> Unregister the thread, the thread is no more
        *   allowed to use get_stream().
        *
        * @exception AssertionFailedException
        *
        */
        void unregister_thread()
            throw (AssertionFailedException);



    protected:


    private:

        // To suppress GCC warning about no Ctor/Dtor: we are a singleton
        friend class foo;

    
        StreamMngr(const StreamMngr& rhs)
                throw();


        StreamMngr&
        operator=(const StreamMngr& rhs)
                throw();

        StreamMngr() 
                throw();


        /**
        * Container used to store association between ThreadId/Stream
        */
        typedef std::map< OsSupport::Thread::ThreadId, Stream* >  StreamMap;


        
        /**
        * Mutex protecting m_streamMap
        */
        OsSupport::Mutex m_streamMap_mtx;



        /**
        * Contains the association between each Stream and their
        * associated thread. When manipulating this list, the m_streamMap_mtx
        * must be acquired.
        *
        */
        StreamMap m_streamMap;
    
	    

}; // End class StreamMngr 

} // End namespace Trace
} // End namespace Cdmw



#endif // INCL_STREAMMNGR_HPP


