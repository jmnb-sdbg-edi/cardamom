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


#include "testtracelibrary/PILOT_FlushArea.hpp"

#include "TraceAndPerf/tracelibrary/Message.hpp"
#include "tracelibrary/FlushArea.hpp"


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


PILOT_FlushArea::PILOT_FlushArea()
    : Testable("Cdmw::FlushArea")
{

}


PILOT_FlushArea::~PILOT_FlushArea()
    throw()
{

}


void 
PILOT_FlushArea::do_tests()
    throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (10);

    size_t areaSize;
    FlushArea::AreaStatus status;

    std::string message1 = "First Message";
    size_t len1 = message1.length();
    std::string message2 = "Second Message";
    size_t len2 = message2.length();
    std::string message3 = "Third Message";
    size_t len3 = message3.length();

	std::string outFlushAreaMsg = "trace Flush Are Manager is running out of flush areas";

    // constructs a message header
    Message::MessageHeader header;
    header.m_fileLine = __LINE__;
    header.m_fileName = __FILE__;
    header.m_userLevel = 1;
    header.m_userDomain = "TEST";
    header.m_tid = OsSupport::Thread::self();
    header.m_timestamping = OsSupport::OS::get_time();

	size_t headerStringSize = header.m_fileName.length() + 1;
	headerStringSize += header.m_userDomain.length() + 1;

 
    TEST_INFO ("Testing the number of messages limit for a flush area");
    FlushArea flushArea1 (2, 500, 100);
    status = flushArea1.add_message (header, message1);

    TEST_CHECK (status == FlushArea::OK);
    status = flushArea1.add_message (header, message2);

    TEST_CHECK (status == FlushArea::AREA_FULL);
    flushArea1.store_outFlushAreaMsg (outFlushAreaMsg);
	TEST_SUCCEED();
    
    TEST_INFO ("Testing the size limit for a flush area");
    areaSize = len1 + len2 + 2;        // Adds 2 for the terminating NUL characters
	areaSize += 2 * headerStringSize;  // Adds 2 header size stored in area    
    FlushArea flushArea2 (3, areaSize, 100);
    status = flushArea2.add_message (header, message1);
    TEST_CHECK (status == FlushArea::OK);
    status = flushArea2.add_message (header, message2);
    
    TEST_CHECK (status == FlushArea::AREA_FULL);
    flushArea2.store_outFlushAreaMsg (outFlushAreaMsg);
	TEST_SUCCEED();

    TEST_INFO ("Trying to exceed the size limit for a flush area");
    areaSize = len1 + len2 + 1;        // Adds 1 for only one terminating NUL characters
	areaSize += 2 * headerStringSize;  // Adds 2 header size stored in area
    FlushArea flushArea3 (3, areaSize, 100);
    status = flushArea3.add_message (header, message1);
    TEST_CHECK (status == FlushArea::OK);    
    status = flushArea3.add_message (header, message2);;
    TEST_CHECK (status == FlushArea::SIZE_TOO_SMALL);
    flushArea3.store_outFlushAreaMsg (outFlushAreaMsg);
	TEST_SUCCEED();

    TEST_INFO ("Trying to add a message too big for a flush area");
    areaSize = len3;               // Adds no extra space for terminating NUL characters
	areaSize += headerStringSize;  // Adds 1 header size stored in area
    FlushArea flushArea4 (3, areaSize, 100);
    status = flushArea4.add_message (header, message3);
    TEST_CHECK (status == FlushArea::MESSAGE_TOO_BIG);

}

}; // End namespace Trace
}; // End namespace Cdmw

