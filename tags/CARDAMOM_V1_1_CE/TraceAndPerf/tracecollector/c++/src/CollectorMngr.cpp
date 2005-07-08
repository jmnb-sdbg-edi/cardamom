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


#include <iostream>
#include <sstream>

#include "tracecollector/CollectorMngr.hpp"
#include "TraceAndPerf/tracelibrary/FilterMngr.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/System.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "tracecollector/TraceCollector_impl.hpp"

#include "tracecollector/TraceCircularBuffer.hpp"
#include "tracecollector/TraceFileMngr.hpp"



namespace Cdmw
{

namespace Trace
{

/**
*Implementation notes:  none 
*<p>
*Portability issues: none
*/


class TraceFileMngr;

/**
*Purpose:
*<p>    This class output the content of trace messages to disk
*
*Features:
*<p> Thread safe, exception safe
*
*@see  TraceCircularBuffer
*@see  TraceFile
*
*/
class CollectorFlusher : public OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param barrier the barrier is used by the internal thread
        *       to notify its creator it is ready to operate
        *@param pCircularBuffer CircularBuffer in charge of bufferize the trace messages
		*@param trace_file_name contains generic name for trace file
		*@param trace_file_bkp_nbr contains number of backup of trace files
	    *@param max_trace_file_nbr contains max number of trace files
	    *@param max_trace_message_nbr contains max number of message in a trace file
	    *@param traceFormat contains format of trace (Horizontal or Vertical)
	    *@param time_mode contains gmt or local mode for display time
        *
        *@exception OutOfMemoryException
        *@exception AssertionFailedException
        *@exception InternalErrorException
        *
        */ 
        CollectorFlusher (OsSupport::Barrier& barrier,TraceCircularBuffer *pCircularBuffer,
						  const std::string& trace_file_base_name, 
						  short trace_file_bkp_nbr,
						  short max_trace_file_nbr, 
						  int max_trace_message_nbr,
						  Message::TraceFormat trace_format,
						  OsSupport::OS::TimeMode time_mode)
                throw (OutOfMemoryException,
                       AssertionFailedException,
                       InternalErrorException,
                       BadParameterException)
                       
                : OsSupport::Thread (),
                  m_thread_must_terminate (false),
                  m_barrier (barrier),
                  m_pCircularBuffer (pCircularBuffer),
	              m_pTraceFileMngr (NULL)
        {
            try
            {
                // get base name for trace file
                std::string trace_file_name = trace_file_base_name;
                
                // if backup of trace files is requested and file name is defined
                if (trace_file_bkp_nbr > 1 && !trace_file_base_name.empty())
                {
                    // build name of backup info
                    std::string backUpInfoFileName = trace_file_base_name;
                    backUpInfoFileName += "_bkpinfo";
                    
                    short current_bkp;
                    
                    // open the file for read backup info
                    std::ifstream bkp_file_in_str (backUpInfoFileName.c_str(), std::ios::in);
                    
                    // if file can be opened
                    if (bkp_file_in_str)
                    {
                      // get current backup from info file
                      bkp_file_in_str >> current_bkp;
                      
                      // if current backup not retrieve (empty file)
                      // set the 1st backup
                      if (!bkp_file_in_str.good())
                      {
                        current_bkp = 1;
                      }
                      
                      // else increment current backup
                      else
                      {
                        current_bkp ++;
                        
                        if (current_bkp > trace_file_bkp_nbr)
                        {
                          current_bkp = 1;
                        }                       
                      }
                      
                      // close the file
                      bkp_file_in_str.close();
                    }
                    else
                    {
                        current_bkp = 1;
                    }
                    
                    
                    // open the file for write or create file if does not exist
                    std::ofstream bkp_file_out_str (backUpInfoFileName.c_str(), std::ios::out);

                    // if file cannot be opened
                    if (!bkp_file_out_str)
                    {
                      CDMW_THROW2(BadParameterException, "Backup Info File cannot be opened",
                                  backUpInfoFileName.c_str());
                    }
                    else
                    {
                      // save current backup nbr in file
                      
                      bkp_file_out_str << current_bkp << std::endl;
                      
                      
                    }
                    
                    // close the file
                    bkp_file_out_str.close();
                    
                    // update the file name base with the backup nbr
                    std::ostringstream name_str;
                    name_str << trace_file_base_name << "_" << current_bkp;
                        
                    trace_file_name = name_str.str();                    
                }
                
                // create trace file manager
                m_pTraceFileMngr = new TraceFileMngr (trace_file_name, 
					                                  max_trace_file_nbr, max_trace_message_nbr,
					                                  trace_format,
										              time_mode);
            }
            catch (const std::bad_alloc&) 
            {
                CDMW_THROW (OutOfMemoryException);
            }
            
        };




        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~CollectorFlusher()
                throw()
        {
            // delete trace file manager
            if (m_pTraceFileMngr != NULL)
            {
                delete m_pTraceFileMngr;
                m_pTraceFileMngr = NULL;
            }
        };




        /**
        * Purpose:
        * <p> Ask to the internal thread to stop its job, the thread
        * will exit only when all trace messages will have been flushed
        * to disk.
        * When this function return the internal thread have stopped.
        *
        *@exception InternalErrorException
        *
        */ 
        void stop_thread()
            throw (InternalErrorException)
        {
            // require the thread to terminate
            m_thread_must_terminate = true;

            // stop the buffer reading -> awake the thread, if it is sleeping
            m_pCircularBuffer->stopReading();

            try 
	        {
                // wait the termination of our internal thread
                this->join();

                // No explanation, how is it possible : corrupted
            } 
	        catch (const OsSupport::DeadlockException&)
	        {
                CDMW_THROW (InternalErrorException);
            }
        };


        
		/**
		* Purpose:
		* <p> print error message in trace file
		* 
		*@param errMesg contains the message to print
		*/ 
		void print_error_message (const std::string& errMesg)
		    throw()
		{
		    m_pTraceFileMngr->print_error_message (errMesg);
		};


    protected:


        /**
        * Purpose:
        * <p> Call by the underlying thread system in a new thread
        * 
        */ 
        virtual
        void run()
            throw()
        {
            try
	        {
                // We notify the CollectorMngr, we are ready
                m_barrier.wait();

	            // message header data buffer
	            TraceCircularBuffer::TraceMessageRaw messageData;

	            // flag to indicate id data read
	            bool dataRead;
	            bool loop = true;

                // Infinite loop
                // Check if someone ask the thread to terminate
                while (loop) 
	            {
                    // read trace message
	                // if no message available thread is set to wait by CircularBuffer
                    bool success = 
                        m_pCircularBuffer->getNextMessage(messageData, dataRead);

		            // if no error and data has been read, print message
	                if (success == true && dataRead == true)
                    {
	  	                m_pTraceFileMngr->print_message (&messageData);
	                }

		            // else if error or thread to terminate, break the loop
		            else if (success == false || m_thread_must_terminate == true)
                    {
                        loop = false;
		            }
                }
      
                // print end of trace message in the current trace file
                m_pTraceFileMngr->print_end_of_trace();
            }
            // catch any exception and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        };


    private:

        //
        // Copy ctor
        //
        CollectorFlusher (const CollectorFlusher& rhs)
                throw();




        //
        // Assignement operator
        //
        CollectorFlusher& operator=(const CollectorFlusher& rhs)
                 throw();


        /**
        * At each iteration the thread test this variable, to
        * see if it must stop it's processing. If true
        * the thread terminates.
        */
        bool m_thread_must_terminate;


        
        /**
        * This barrier is used to be synchronised with the CollectorMngr.
        * By signalling this barrier we are able to notify it, when the
        * thread is operational.
        */
        OsSupport::Barrier& m_barrier;           

		/**
		* Hold circular buffer
		*/
		TraceCircularBuffer*  m_pCircularBuffer;

		/**
		* Hold Trace File manager
		*/
		TraceFileMngr*  m_pTraceFileMngr;


}; // End class CollectorFlusher 



/**
* define default trace file name
*/
const char* CollectorMngr::DEFAULT_TRACE_FILE="./cdmw_trace";
        
/**
* This variable is set to true, when the CollectorMngr::Init() function
* has been called.
*/
bool CollectorMngr::M_initDone = false;

/**
* Contains the CollectorMngr singleton initialised by the 
* CollectorMngr::Init() function.
*/
CollectorMngr* CollectorMngr::M_pSingleton = NULL;



//
// Ctor
//
// Note:
// m_barrier (counter set to 2) 
// The CollectorMngr constructor and the CollectorFlusher thread 
// have to reach their respective waiting point to continue
// themselves their treatments
//
CollectorMngr::CollectorMngr (PortableServer::POA_ptr tracePOA,
				              const std::string& domain, 
							  long level,
							  Message::TraceFormat traceFormat,							  
							  Message::CircularMode queueStrategy,
							  size_t queueSize,
							  size_t stringSize,
		                      const std::string& trace_file_name, 
		                      short trace_file_bkp_nbr,
					          short max_trace_file_nbr, 
					   		  int max_trace_message_nbr,					   		  
							  OsSupport::OS::TimeMode time_mode)
    throw (OutOfMemoryException,
		   BadParameterException,
		   InternalErrorException)
           
    : m_spCollectorFlusher(NULL),
      m_barrier(2),
	  m_pFilterMngr(NULL),
	  m_pCircularBuffer(NULL)
{

    try 
	{
        // set sizes from KBytes to Bytes
	    queueSize *= 1024;
	    stringSize *= 1024;

        // We create the object in charge of determining, if a specific level/domain must be traced
        m_pFilterMngr = new FilterMngr();

		// initialize the domain and level to trace
		m_pFilterMngr->activate_level (domain, level);
		
        // We create the object in charge of circular buffer
        m_pCircularBuffer = new TraceCircularBuffer(queueSize,stringSize,queueStrategy);

        // We create the thread in charge of flushing Trace Message as a background task

#       ifdef _MSC_VER
	    // The auto_ptr implementation shipped with visual is not compliant
	    //	and doesn't provide reset(), but it broken as it used an assignement
	    //	operator with const auto_ptr&, we use this deviation

        // We create the thread in charge of communicating with collectors
        m_spCollectorFlusher = 
				std::auto_ptr <CollectorFlusher> (new CollectorFlusher(m_barrier, 
										                               m_pCircularBuffer,
								                                       trace_file_name,
								                                       trace_file_bkp_nbr,
														               max_trace_file_nbr, 
																	   max_trace_message_nbr,
																	   traceFormat,
																	   time_mode));
#	    else
        m_spCollectorFlusher.reset (new CollectorFlusher (m_barrier,m_pCircularBuffer,
								                          trace_file_name,
								                          trace_file_bkp_nbr,
														  max_trace_file_nbr, 
														  max_trace_message_nbr,
														  traceFormat,
														  time_mode));
#	    endif

        // We must create the Servant to handle registration request (ref count is incremented)
        TraceCollector_impl* pTraceCollectorServant = 
				               new TraceCollector_impl (this, m_pFilterMngr); 
    
		// create an object var to take pointer ownership
		// (ref count will be decremented when var will be destroyed at the method end)
		PortableServer::ServantBase_var servant_var = pTraceCollectorServant;
		
		// create an object id
		PortableServer::ObjectId_var oid 
				            = PortableServer::string_to_ObjectId("TraceCollector");
		try
        {
		  // activate servant on POA (ref count is incremented)
		  tracePOA->activate_object_with_id(oid.in(),pTraceCollectorServant);
		  CORBA::Object_var object = tracePOA->id_to_reference(oid.in());
		  m_traceCollector = CdmwTrace::Collector::_narrow(object.in());
        }
	    catch (const PortableServer::POA::WrongPolicy &)
	    {
	      CDMW_THROW2(Cdmw::BadParameterException, "Wrong POA Policy for activate_object", "NULL");
	    }
		catch (const PortableServer::POA::ObjectAlreadyActive & )
		{
		  CDMW_THROW2(BadParameterException, "Trace Collector Object already active", "NULL"); 
	    } 
		catch (const PortableServer::POA::ServantAlreadyActive & )
		{
		  CDMW_THROW2(BadParameterException, "Trace Collector Servant already active", "NULL");  
        }
		catch (const CORBA::Exception& ex)
	    {
	      std::cerr << "Trace Collector servant activation error" << "\n";
	      std::cerr << "CORBA Exception : \n" << ex << std::endl;
		  CDMW_ASSERT(false);
	    }

		try
        {
          // Ok, we start the background thread in charge of fluhsing data
          m_spCollectorFlusher->start();

          // Before returning, we wait it declares itself as operational
          m_barrier.wait();
        }
        // We have not choose to assign specific right to the thread
        //  we are corrupted
        catch (const OsSupport::ThreadSchedulingException&)
	    {
          CDMW_THROW (InternalErrorException);
        }
    } 
	catch (const std::bad_alloc&) 
	{
      CDMW_THROW (OutOfMemoryException);
    }
}



//
// Initialise the trace collector
//
CdmwTrace::Collector_ptr 
CollectorMngr::Init (PortableServer::POA_ptr tracePOA,
				     const std::string& domain, 
					 long level,
					 Message::TraceFormat trace_format,
					 Message::CircularMode queueStrategy,
					 size_t queueSize,
					 size_t stringSize,
		             const std::string& trace_file_name, 
		             short trace_file_bkp_nbr,
					 short max_trace_file_nbr, 
					 int max_trace_message_nbr,
					 OsSupport::OS::TimeMode time_mode)
    throw (OutOfMemoryException,
		   BadParameterException,
		   InternalErrorException)
{

    // The user try to initialise twice the collector
    CDMW_ASSERT (M_initDone==false);

    
    try 
	{
        M_pSingleton = new CollectorMngr(tracePOA,
				                         domain, 
					                     level,
					                     trace_format,
						                 queueStrategy,
                                         queueSize,
                                         stringSize,
		                                 trace_file_name,
		                                 trace_file_bkp_nbr, 
					                     max_trace_file_nbr, 
					                     max_trace_message_nbr,
					                     time_mode);
    } 
	catch (const std::bad_alloc&)
	{
      CDMW_THROW (OutOfMemoryException);
    } 

    M_initDone = true;

    // We return the reference on the object managing communication with 
    //  collector, it will be registered in the repository by the caller
    return CdmwTrace::Collector::_duplicate(M_pSingleton->m_traceCollector.in());
}




//
// Free memory and terminate the Trace service
//
void CollectorMngr::Cleanup()
    throw (InternalErrorException)
{
    // The library is not initialised
    CDMW_ASSERT (M_initDone==true);

    // We stop the thread in charge of flushing data
    M_pSingleton->m_spCollectorFlusher->stop_thread();

    M_initDone = false;
    delete M_pSingleton;
    
    M_pSingleton = NULL;
}



//
// Dtor
// 
CollectorMngr::~CollectorMngr()
    throw()
{
    if (m_pFilterMngr != NULL)
	{
      delete m_pFilterMngr;
      m_pFilterMngr = NULL;
    }

    if (m_pCircularBuffer != NULL)
	{
      delete m_pCircularBuffer;
      m_pCircularBuffer = NULL;
    }
     
    // The user has not called the Cleanup function
    CDMW_ASSERT (M_initDone == false);
}




CollectorMngr& CollectorMngr::Instance()
    throw ()
{
    // The user has not called the Init() function
    CDMW_ASSERT (M_initDone == true);
    
    return *M_pSingleton ;
}



// 
// write message in the circular buffer to be flushed on disk
//
bool CollectorMngr::writeMessage (const CdmwTrace::MessagesHeader& messagesHdr,
				                  const CdmwTrace::FormattedMessage& fmtMessage)
     throw (OutOfMemoryException)
{
	bool ret = true;

    CDMW_ASSERT (m_spCollectorFlusher.get() != NULL);
    CDMW_ASSERT (m_pCircularBuffer != NULL);

	// check if trace message is filtered by domain and level
    std::string domain = fmtMessage.header.the_level.the_domain.in();
    if (CollectorMngr::Is_to_be_traced (domain, fmtMessage.header.the_level.the_value))
    {
	  // if not filtered, write message in circular buffer
	  // collector flusher is signaled to flush message on disk
	  TraceCircularBuffer::WriteStatus write_status;
	  ret = m_pCircularBuffer->writeMessage (messagesHdr, fmtMessage, write_status);

	  if  (write_status == TraceCircularBuffer::STS_DROP_LATEST)
      {
        m_spCollectorFlusher->print_error_message(
						"Queue is full : latest trace message has been dropped");
      }
	  else if  (write_status == TraceCircularBuffer::STS_REMOVE_OLDEST)
      {
        m_spCollectorFlusher->print_error_message(
						"Queue is full : oldest trace message has been removed");
      }
    }

    return ret;
}


//
// returns true if the specified level is to be traced
//
bool CollectorMngr::Is_to_be_traced (const std::string& domain, long level)
{
    
    // The user has not called CollectorMngr::Init() before
    CDMW_ASSERT (M_initDone==true);

    return M_pSingleton->m_pFilterMngr->is_activated (domain, level);
}    


}; // End namespace Trace
}; // End namespace Cdmw

