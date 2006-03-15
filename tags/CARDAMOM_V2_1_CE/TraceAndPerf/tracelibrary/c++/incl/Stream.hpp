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


#ifndef INCL_TRACE_STREAM_HPP 
#define INCL_TRACE_STREAM_HPP 

#include <sstream>
#include <string>

#include "Foundation/common/Exception.hpp"

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
 * Purpose:
 * <p>This class is used to construct message in one thread.
 *    When the message has been fully built, a call to do_flush(),
 *    will transfer its content into another memory to be flushed out
 *    to each collectors interested in it. As Stream class is only
 *    intended to be used by one thread at a time, it doesn't have
 *    any protection against race condition.
 *
 * Features:
 * <p> No Thread safe, exception safe
 *
 * @see CDMW_TRACE
 * @see CDMW_TRACE_CCM
 * @see CDMW_TRACE_ST
 * @see CDMW_TRACE_ST_CCM
 * @see CDMW_REGISTER_THREAD
 * @see CDMW_UNREGISTER_THREAD
 */
class Stream : public std::ostringstream
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        Stream() 
            throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~Stream()
            throw();




        /**
        * Purpose:
        * <p> Set the line origin of the trace
        * 
        *@param line the line number
	    *
        */ 
	    void set_line (size_t line)
	        throw();
		
		
        /**
        * Purpose:
        * <p> Set the file  origin of the trace
        * 
        *@param file the file name
        *
        *@exception OutOfMemoryException
        *
        */ 
	    void set_file (const std::string& file)
	        throw (OutOfMemoryException);


        /**
         * Set the name of the component.
         *
         * @exception OutOfMemoryException
         */
        // ECR-0123
        void
        set_component_name(const std::string& componentName)
            throw(OutOfMemoryException);


        /**
        * Purpose:
        * <p> Set the user domain from where the trace comes from
        * 
        *@param domain the user domain
        *
        *@exception OutOfMemoryException
        *
        */ 
	    void set_userDomain (const std::string& domain)
	        throw (OutOfMemoryException);


        
        
        /**
        * Purpose:
        * <p> Set the user level from where the trace comes from
        * 
        *@param level the user level
        *
        */ 
	    void set_userLevel (int level)
	        throw();



        /**
        * Purpose:
        * <p> Transfer the internal message in another memory area
        *     to be flushed out to the Collectors. The stream is
        *     reset to "", so the same stream can be reused for 
        *     other outputs.
        *
        * @exception OutOfMemoryException
        *
        */
        void do_flush()
	        throw (OutOfMemoryException);


        
    protected:


    private:

    
        /**
        * Purpose:
        * <p>  Copy constructor 
        * 
        */ 
        Stream(const Stream& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator 
        * 
        */ 
        Stream&
        operator=(const Stream& rhs)
                throw();

    
    
        /**
	    * Line number where the trace begun
	    */
	    size_t m_line;
    
	    /**
	    * File name where the trace has been produced
	    */	    
	    std::string m_file;


        /**
         * The component name.
         */
        // ECR-0123
        std::string m_componentName;


	    /**
	    * User domain as specified
	    */	    
	    std::string m_userDomain;


        
        /**
	    * User level as specified
	    */
	    int m_userLevel;
	    

}; // End class Stream 



} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_STREAM_HPP

