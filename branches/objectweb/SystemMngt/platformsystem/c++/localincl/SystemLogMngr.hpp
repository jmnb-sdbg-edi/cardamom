/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_SYSTEMLOGMNGR_HPP 
#define INCL_PLATFORMMNGT_SYSTEMLOGMNGR_HPP


#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "Foundation/common/Exception.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

    
class SystemLogMngr : public LogMngr
{
    public:

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~SystemLogMngr()
            throw();
            
        /**
         *Purpose:
         *<p>
         * Initialises the logging manager with standard output.
         *
         *@exception OutOfMemoryException
         */
        static void initialise()
               throw (OutOfMemoryException);
                   
        /**
         *Purpose:
         *<p>
         * Initialises the logging manager with log file.
         *
         *@param logFile the path of the log file.
         *
         *@exception OutOfMemoryException
         */
        static void initialise(const std::string& logFile)
               throw (OutOfMemoryException);
        
        /**
         *Purpose:
         *<p>
         * Returns the identifier of the service.
         */
        virtual const char* getService();


        /**
         *Purpose:
         *<p>
         * Returns the specified message string or NULL if messageId is invalid.
         *
         * @param messageId the identifier of the message.
         */
        virtual const char* getMessage(long messageId);
        
        

    private:
    
        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        SystemLogMngr()
            throw();
 
            

}; // End class SystemLogMngr 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_SYSTEMLOGMNGR_HPP


