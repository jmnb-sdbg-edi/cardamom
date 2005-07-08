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


#include "Foundation/ossupport/CircularFile.hpp"

#include <sstream>

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: 
        *<p>
        *Portability issues:  none 
        *<p>
        */





        //
        // Ctor
        //

        CircularFile::CircularFile (const std::string& file_name,
                                    short max_file_nbr, int max_message_nbr)
        throw(OutOfMemoryException,
              BadParameterException)
                : m_pCircularFileStream (NULL),
                m_CircularFileNumber(0),
                m_MessageNumber(0),
                m_MaxCircularFileNumber(max_file_nbr),
                m_MaxMessageNumber(max_message_nbr)
        {
            try
            {
                // get circular file name
                m_CircularFileBaseName = file_name;

                // if file name is defined

                if (!file_name.empty())
                {
                    // set first circular file name
                    m_CircularFileName = file_name;
                    m_CircularFileName += ".0";

                    // Create and open first circular file
                    m_pCircularFileStream = new std::ofstream(m_CircularFileName.c_str(), std::ios::out);

                    // Create following trace files

                    for (int i = 1 ; i < max_file_nbr ; i++)
                    {
                        // create name of file with index
                        std::ostringstream name_str;
                        name_str << file_name << "." << i;

                        // create the file
                        std::ofstream file_str (name_str.str().c_str(), std::ios::out);

                        if (!file_str)
                        {
                            CDMW_THROW2(BadParameterException, "Circular File cannot be opened",
                                         name_str.str().c_str());
                        }
                        else
                        {
                            file_str.close();
                        }
                    }
                }
            }
            catch (const std::bad_alloc&)
            {
                CDMW_THROW (OutOfMemoryException);
            }
        }


        //
        // Dtor
        //

        CircularFile::~CircularFile()
        throw()
        {

            // Close the current circular file and delete the stream

            if (m_pCircularFileStream != NULL)
            {
                m_pCircularFileStream->close();

                delete m_pCircularFileStream;
                m_pCircularFileStream = NULL;
            }
        }



        //
        // Get the current Circular File stream to write in a new message
        //     return std::cout if stream is NULL
        //     return NULL if stream error
        //     return file stream pointer in other cases
        //

        std::ostream* CircularFile::getCurrentCircularFileToWrite () const
        {
            std::ostream* fileStream = NULL;

            if (m_pCircularFileStream == NULL)
            {
                fileStream = &std::cout;
            }
            else if (m_pCircularFileStream->good())
            {
                fileStream = m_pCircularFileStream;
            }

            return fileStream;
        }

        //
        // Determine the current Circular File to write in a new message
        //   if the max number of messages is reached in circular file,
        //   it is closed and a new one is opened
        //     return std::cout if stream is NULL
        //     return NULL if stream error
        //     return file stream pointer in other cases
        //
        std::ostream* CircularFile::determineCircularFileToWrite ()
        throw()
        {
            std::ostream* fileStream = NULL;

            if (m_pCircularFileStream == NULL)
            {
                return &std::cout;             // --> return cout if no file stream
            }

            // A new message has to be written so increment the number of messages
            m_MessageNumber++;

            // If current circular file is full of messages, create a new file
            if (m_MessageNumber > m_MaxMessageNumber)
            {
                // Close current circular file
                m_pCircularFileStream->close();

                // Reset message counter
                m_MessageNumber = 1;

                // A new file is to be written
                m_CircularFileNumber++;

                // Take into account cyclic file name

                if (m_CircularFileNumber == m_MaxCircularFileNumber)
                {
                    if (m_MaxCircularFileNumber > 1)
                    {
                        m_CircularFileNumber = 1;
                    }
                    else
                    {
                        m_CircularFileNumber = 0;
                    }
                }

                // create name of file with index
                std::ostringstream name_str;

                name_str << m_CircularFileBaseName << "." << m_CircularFileNumber;

                m_CircularFileName = name_str.str();

                // open the following circular file
                m_pCircularFileStream->open(m_CircularFileName.c_str(), std::ios::out);
            }

            // if no stream error
            if (m_pCircularFileStream->good())
            {
                fileStream = m_pCircularFileStream;
            }

            return fileStream;
        }



        //
        //Print Error message in trace file
        //
        void CircularFile::print_error_message (const std::string& errMesg)
        throw()
        {
            try
            {
                // set mutex for trace file stream updating
                CDMW_MUTEX_GUARD (m_lock);

                // determine the circular file to write
                std::ostream* p_os = determineCircularFileToWrite();

                // if no stream error

                if (p_os != NULL)
                {
                    // print error message
                    *p_os << "Error Message :" << errMesg.c_str() << std::endl;

                }
            }

            // catch any exception from Mutex and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }



        //
        //Format and Print Trace message
        //
        void CircularFile::print_message (const std::string& Mesg)
        throw()
        {


            try
            {
                // set mutex for trace file stream updating
                CDMW_MUTEX_GUARD (m_lock);


                // determine the circular file to write
                std::ostream* p_os = determineCircularFileToWrite();

                // if no stream error

                if (p_os != NULL)
                {
                    // print message
                    *p_os << Mesg.c_str() << std::endl;

                }
            }

            // catch any exception and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }



    } // End namespace OsSupport
} // End namespace Cdmw

