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


#ifndef INCL_OSSUPPORT_CIRCULARFILE_HPP
#define INCL_OSSUPPORT_CIRCULARFILE_HPP

#include <string>
#include <iostream>
#include <fstream>

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OsSupport types and data.
    */

    namespace OsSupport
    {


        /**
         *Purpose:
         *<p>    This class manages the opened circular file where messages are written
         *
         *Features:
         *<p> Thread safe, exception safe
         *
         *@see
         *
         */

        class CircularFile
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                * 
                *@param file_name contains generic name for circular file
                *@param max_file_nbr contains max number of circular files
                *@param max_message_nbr contains max number of message in a circular file
                *
                *@exception OutOfMemoryException
                *@exception BadParameterException
                *
                */
                CircularFile (const std::string& file_name,
                              short max_file_nbr, int max_message_nbr)
                throw(OutOfMemoryException,
                      BadParameterException);

                /**
                * Purpose:
                * <p> Destructor
                * 
                */
                virtual
                ~CircularFile ()
                throw();


                /**
                * Purpose:
                * <p> get the current Circular File Stream to write a message
                * 
                *@param
                *@return std::cout if current file stream is NULL
                *        NULL if current file stream error
                *        pointer to current file stream in other cases
                *@exception
                */
                std::ostream* getCurrentCircularFileToWrite () const;

                /**
                * Purpose:
                * <p> determine the circular file to write the message
                *     if max number of messages is reached, new circular file
                *     is returned else the current file is returned
                * 
                *@return std::cout if file stream is NULL
                *        NULL if file stream error
                *        pointer to file stream in other cases
                *
                *@exception
                *
                */
                std::ostream* determineCircularFileToWrite ()
                throw();

                /**
                * Purpose:
                * <p> get the current Circular File Stream
                * 
                *@param
                *@return A stream pointer to the current circular file
                *@exception
                */
                std::ofstream* getCircularFileStream () const;

                /**
                * Purpose:
                * <p> get the current Circular File Name
                * 
                *@param
                *@return A string reference to the current circular file name
                *@exception
                */
                const std::string& getCircularFileName () const;

                /**
                * Purpose:
                * <p> get the current Circular File number
                * 
                *@param
                *@return the file number
                *@exception
                */
                const int getCircularFileNumber () const;

                /**
                * Purpose:
                * <p> get the current Message number in current Circular File
                * 
                *@param
                *@return the number of messages
                *@exception
                */
                const int getMessageNumber () const;

                /**
                * Purpose:
                * <p> get the max Message number in Circular File
                * 
                *@param
                *@return the max number of messages
                *@exception
                */
                const int getMaxMessageNumber () const;


                /**
                * Purpose:
                * <p> print error message in circular file
                * 
                *@param errMesg contains the message to print
                */
                void print_error_message (const std::string& errMesg)
                throw();

                /**
                * Purpose:
                * <p> print message in circular file
                * 
                *@param Mesg contains the message to print
                */
                void print_message (const std::string& Mesg)
                throw();




            private:

                /**
                * Circular file stream
                */
                std::ofstream* m_pCircularFileStream;

                /**
                * Circular file basename
                */
                std::string m_CircularFileBaseName;

                /**
                * Circular file name
                */
                std::string m_CircularFileName;

                /**
                * Circular file number
                */
                short m_CircularFileNumber;

                /**
                * Message number
                */
                int m_MessageNumber;

                /**
                * Max circular file number
                */
                short m_MaxCircularFileNumber;

                /**
                * Max message number
                */
                int m_MaxMessageNumber;

                /**
                * lock for trace file stream access
                */
                Mutex m_lock;




        }

        ; // End class CircularFile

        //## Get and Set Operations for Class Attributes (inline)

        inline std::ofstream* CircularFile::getCircularFileStream () const
        {
            return m_pCircularFileStream;
        }

        inline const std::string& CircularFile::getCircularFileName () const
        {
            return m_CircularFileName;
        }

        inline const int CircularFile::getCircularFileNumber () const
        {
            return m_CircularFileNumber;
        }

        inline const int CircularFile::getMessageNumber () const
        {
            return m_MessageNumber;
        }

        inline const int CircularFile::getMaxMessageNumber () const
        {
            return m_MaxMessageNumber;
        }


    } // End namespace OsSupport
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_CIRCULARFILE_HPP

