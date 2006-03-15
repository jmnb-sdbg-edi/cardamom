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


#include "tracecollector/TraceFileMngr.hpp"

#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include <sstream>

namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/


static const char* end_of_trace_file  = "--------------- END OF TRACE FILE ----------------";
static const char* trace_header1      = "**************************************************";
static const char* trace_header2      = "                   LOG FILE ";
static const char* trace_header3      = " (Cont)";
static const char* trace_err_mesg1    = "--------------------------------------------------";
static const char* trace_err_mesg2    = "---------- Error Message from Trace --------------";

//
// Ctor
// 

TraceFileMngr::TraceFileMngr (const std::string& file_name, 
                              short max_file_nbr, int max_message_nbr,
                              Message::TraceFormat trace_format,
                              OsSupport::OS::TimeMode time_mode)
              throw(OutOfMemoryException,
                    BadParameterException)
      : CircularFile (file_name, max_file_nbr, max_message_nbr),
        m_trace_format (trace_format),
        m_time_mode (time_mode)
{
}


//
// Dtor
// 

TraceFileMngr::~TraceFileMngr()
        throw()
{
    
}


// return the circular trace file to write a new log message
// if a new circular file is opened due to max number of messages
// in previous file, end message and header are written in

std::ostream* TraceFileMngr::determineTraceFile ()
              throw(BadParameterException)
{
    // get current trace circular file stream
    std::ostream* pTraceFileStream = getCurrentCircularFileToWrite();

    // if circular file defined (not std::cout) and no stream error
    if (getCircularFileStream () != NULL && pTraceFileStream != NULL)
    {
      // If current trace file is full of messages, write end message
      if (getMessageNumber() == getMaxMessageNumber())
      {
        // Write end message
        *pTraceFileStream << end_of_trace_file << std::endl;
      }
    }
    
    // determine the circular file to write :
    // the current or a new trace circular file stream if max messages reached
    pTraceFileStream = determineCircularFileToWrite ();

    // if circular file defined (not std::cout)
    if (getCircularFileStream () != NULL)
    {
      // if no stream error
      if (pTraceFileStream != NULL)
      {
        // If current trace file is empty, write header message
        if (getMessageNumber() == 1)
        {
            // Write a header in the log file
          *pTraceFileStream << trace_header1 << std::endl;

          *pTraceFileStream << trace_header2;
          if (getCircularFileNumber() > 0)
          {
              *pTraceFileStream << getCircularFileNumber() << trace_header3;
          }
          *pTraceFileStream << std::endl;

          *pTraceFileStream << trace_header1 << std::endl;
          *pTraceFileStream << std::endl;
        }
      }
      else
      {
        CDMW_THROW2(BadParameterException,"Trace Log File cannot be opened",
                     getCircularFileName().c_str());
      }
    }
  
    return pTraceFileStream;
}

//
// Print Error message in trace file
// 
void TraceFileMngr::print_error_message (const std::string& errMesg)
    throw()
{
    try
    {
      // set mutex for trace file stream updating
      CDMW_MUTEX_GUARD (m_lock);
  
      // determine the openned trace file stream
      std::ostream* p_os = determineTraceFile();

      // if no stream error
      if (p_os != NULL) 
      {
        // print error message
        *p_os << trace_err_mesg1 << std::endl;
        *p_os << trace_err_mesg2 << std::endl;
        *p_os << errMesg << std::endl;
        *p_os << trace_err_mesg1 << std::endl;
        *p_os << std::endl;
      }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }
}



//
// Format and Print Trace message
// 
void TraceFileMngr::print_message (TraceCircularBuffer::TraceMessageRaw* pData)
    throw()
{
    struct TraceCircularBuffer::TraceMainHeaderRaw* p_mainHeader = &(pData->m_mainHeader);
    struct Message::MessageHeaderRaw* p_messageHeader = &(pData->m_messageHeader);


    try
    {
      // set mutex for trace file stream updating
      CDMW_MUTEX_GUARD (m_lock);
  
      // format date
      char time_buf[128];
	  OsSupport::OS::get_fmt_time (time_buf, sizeof(time_buf), 
                        p_messageHeader->m_timestamping.seconds,
                        "%d %m %Y %H:%M:%S", m_time_mode);
	              
      // determine the current openned trace file stream
      std::ostream* p_os = determineTraceFile();

      // if no stream error
      if (p_os != NULL) 
      {
        // if vertical format
        if (m_trace_format == Message::V_FORMAT)
        {
          // format and print message
          *p_os << "Date             : " << time_buf << std::endl;
          *p_os << "Time Stamp       : " << p_messageHeader->m_timestamping.seconds;
          *p_os << " seconds  "      << p_messageHeader->m_timestamping.microseconds << " microseconds" << std::endl;

          *p_os << "Host Name        : " << p_mainHeader->m_pHostName << std::endl;
          *p_os << "Application Name : " << p_mainHeader->m_pApplicationName << std::endl;
          *p_os << "Process Name     : " << p_mainHeader->m_pProcessName << std::endl;
          *p_os << "Process Id       : " << p_mainHeader->m_pid << std::endl;
          // ECR-0123
          *p_os << "Component Name   : " << p_messageHeader->m_pComponentName << std::endl;
          *p_os << "Domain           : " << p_messageHeader->m_pUserDomain << std::endl;
          *p_os << "Level            : " << p_messageHeader->m_userLevel << std::endl;
          *p_os << "Thread Id        : " << p_messageHeader->m_tid << std::endl;
          *p_os << "File Name        : " << p_messageHeader->m_pFileName << std::endl;
          *p_os << "Line             : " << p_messageHeader->m_fileLine << std::endl;
          *p_os << "==>" << std::endl;

          *p_os << pData->m_pBodyMessage << std::endl;
          *p_os << std::endl;
        }
    
        // else horizontal format
        else
        {
            char string_buf[128];
            
            sprintf (string_buf, "%-19.19s", time_buf);
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%012ld", p_messageHeader->m_timestamping.seconds);
            *p_os << string_buf << ".";
            sprintf (string_buf, "%-6ld", p_messageHeader->m_timestamping.microseconds);
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%-12.12s", p_mainHeader->m_pHostName);
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%-16.16s", p_mainHeader->m_pApplicationName);
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%-16.16s", p_mainHeader->m_pProcessName);
            *p_os << string_buf << " ";
            
            {
              std::ostringstream stream_buf;
              
              stream_buf << p_mainHeader->m_pid;
              sprintf (string_buf, "%-5.5s", stream_buf.str().c_str());
              *p_os << string_buf << " ";
            }

            // ECR-0123
            sprintf(string_buf, "%-16.16s", p_messageHeader->m_pComponentName);
            *p_os << string_buf << " ";

            sprintf (string_buf, "%-16.16s", p_messageHeader->m_pUserDomain);
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%-5d", p_messageHeader->m_userLevel);
            *p_os << string_buf << " ";
            
            {
              std::ostringstream stream_buf;
              
              stream_buf << p_messageHeader->m_tid;
              sprintf (string_buf, "%-5.5s", stream_buf.str().c_str());
              *p_os << string_buf << " ";
            }
            
            // extract the file name from the full path
            std::string file_name = p_messageHeader->m_pFileName;
            int inx = file_name.rfind ('/');
            if (inx < 0 || (unsigned) inx > file_name.size())
            {
                inx = 0;
            }
            else
            {
                inx ++;
            }
            
                       
            sprintf (string_buf, "%-16.16s", file_name.substr(inx).c_str());
            *p_os << string_buf << " ";
            
            sprintf (string_buf, "%-5.5d", p_messageHeader->m_fileLine);
            *p_os << string_buf << " ";
            
            *p_os << "==>";
            *p_os << pData->m_pBodyMessage << std::endl;
        }
        
      }
    }

    // catch any exception and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }
}

//
// Print end of trace file message
// 
void TraceFileMngr::print_end_of_trace () const
    throw()
{
    // Write end message in current log file

    // if circular file defined (not std::cout)
    // write the end of trace file
    if (getCircularFileStream () != NULL)
    {
      // get openned trace file stream
      std::ostream* pTraceFileStream = getCurrentCircularFileToWrite();

      if (pTraceFileStream != NULL)
      {
        // if no open error write end message
        *pTraceFileStream << end_of_trace_file << std::endl;
      }
    }
}

}; // End namespace Trace
}; // End namespace Cdmw

