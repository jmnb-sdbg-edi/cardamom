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


#ifndef INCL_TRACE_TRACEFILEMNGR_HPP 
#define INCL_TRACE_TRACEFILEMNGR_HPP 

#include <string>
#include <iostream>

#include "Foundation/common/Exception.hpp"

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/ossupport/CircularFile.hpp"
#include "tracecollector/TraceCircularBuffer.hpp"

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


/**
 *Purpose:
 *<p>    This class manages the opened trace file where messages are written
 *
 *Features:
 *<p> Thread safe, exception safe
 *
 *@see
 *
 */

class TraceFileMngr : public OsSupport::CircularFile
{
  public:


      /**
      * Purpose:
      * <p> Constructor
      * 
      *@param file_name contains generic name for trace file
      *@param max_file_nbr contains max number of trace files
      *@param max_message_nbr contains max number of message in a trace file
      *@param traceFormat contains format of trace (Horizontal or Vertical)
      *@param time_mode contains the mode of time gmt or local
      *
      *@exception OutOfMemoryException
      *@exception BadParameterException
      *
      */
      TraceFileMngr (const std::string& file_name, 
                     short max_file_nbr=1, int max_message_nbr=100,
                     Message::TraceFormat trace_format=Message::V_FORMAT,
                     OsSupport::OS::TimeMode time_mode=OsSupport::OS::LOCAL_TIME)
              throw(OutOfMemoryException,
                    BadParameterException);

      /**
      * Purpose:
      * <p> Destructor
      * 
      */
      virtual
      ~TraceFileMngr ()
              throw();


      /**
      * Purpose:
      * <p> print error message in trace file
      * 
      *@param errMesg contains the message to print
      */ 
      void print_error_message (const std::string& errMesg)
                                throw();

      /**
      * Purpose:
      * <p> format and print trace message
      * 
      *@param pData contains the reference to trace message to print
      */ 
      void print_message (TraceCircularBuffer::TraceMessageRaw* pData)
                                throw();
                                
      /**
      * Purpose:
      * <p> print end of trace file
      * 
      */ 
      void print_end_of_trace () const
                                throw();


  protected:

      /**
      * Purpose:
      * <p> determine the trace circular file stream to write the trace message
      * 
      *@return std::cout if current file stream is NULL
      *        NULL if current file stream error
      *        pointer to current file stream in other cases
      *
      *@exception BadParameterException
      *
      */
      std::ostream* determineTraceFile ()
              throw(BadParameterException);

  private:

      /**
      * Purpose:
      * <p>  Copy constructor
      * 
      *@param
      *@exception
      */ 
      TraceFileMngr (const TraceFileMngr& rhs)
              throw();

      /**
      * Purpose:
      * <p>  Assignement operator
      * 
      *@param
      *@exception
      */ 
      TraceFileMngr& operator=(const TraceFileMngr& rhs)
              throw();
            
      /**
      * lock for trace file stream access
      */
      mutable OsSupport::Mutex m_lock;

   
      /**
      * Hold trace format display Horizontal or Vertical
      */
      Message::TraceFormat m_trace_format;

      /**
      * Hold time mode display gmt or local
      */
      OsSupport::OS::TimeMode m_time_mode;



}; // End class TraceFileMngr 

}; // End namespace Trace
}; // End namespace Cdmw

#endif // INCL_TRACE_TRACEFILEMNGR_HPP

