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


#include "testtracelibrary/PILOT_StreamMngr.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "TraceAndPerf/tracelibrary/StreamMngr.hpp"

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
class StreamMngrTestThread : public OsSupport::Thread
{

    public :

        StreamMngrTestThread()
        {
        }

        bool is_sameStream(Stream* pAddrStream)
        {
          return m_my_private_streamAddr == pAddrStream;
        }


    protected:

        virtual
        void run()
            throw()
        {
          StreamMngr::instance().register_thread();

          // We obtain a private stream and keeping its address
          //  to compare later
          Stream& my_stream = StreamMngr::instance().get_stream();
          m_my_private_streamAddr = &my_stream;
        }
        

    private:
        Stream* m_my_private_streamAddr; 

        
};


PILOT_StreamMngr::PILOT_StreamMngr()
    : Testable("Cdmw::StreamMng")
{
    
}


PILOT_StreamMngr::~PILOT_StreamMngr()
    throw()
{

}


void
PILOT_StreamMngr::do_tests()
    throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (5);

    TEST_INFO ("Trying to register the thread into the StreamManger");
    StreamMngr::instance().register_thread();
    TEST_SUCCEED();

    TEST_INFO ("Trying to obtain the thread specific stream");
    Stream& myStream = StreamMngr::instance().get_stream();
    TEST_SUCCEED();

    TEST_INFO ("Trying to get a second time the stream");
    Stream& myStream2 = StreamMngr::instance().get_stream();
    TEST_CHECK (&myStream == &myStream2);

    TEST_INFO ("Creating a thread asking for another stream");
    StreamMngrTestThread streamMngrTestThread;
    streamMngrTestThread.start();

    TEST_INFO ("Checking the Stream allocated to the stream is");
    TEST_INFO (" different from our");
    TEST_CHECK (!streamMngrTestThread.is_sameStream (&myStream));

    TEST_INFO ("Trying to unregister our own thread");
    StreamMngr::instance().unregister_thread();
    TEST_SUCCEED();

    streamMngrTestThread.join();
}    

}; // End namespace Trace
}; // End namespace Cdmw

