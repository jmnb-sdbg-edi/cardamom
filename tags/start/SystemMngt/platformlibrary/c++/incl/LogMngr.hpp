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


#ifndef INCL_PLATFORMMNGT_LOGMNGR_HPP 
#define INCL_PLATFORMMNGT_LOGMNGR_HPP

#include <iostream>
#include <string>
#include <cstdio>

#include <sstream>
#include <fstream>
#include <ctime>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
 *Purpose:
 *<p>
 * The type of log.
 */
enum LogType
{
    WRN,
    INF,
    ERR,
    FTL
};


/**
*Purpose:
*<p> 
* The embedded Logging Manager.
*
* The definition of the getMessage() and getService() methods must be provided
* by the process embedding the Logging Manager.
*
*Features:
*<p> Thread safe. Exception safe.
*/        
class LogMngr 
{

    static const char *WRN_STRING;
    static const char *INF_STRING;
    static const char *ERR_STRING;
    static const char *FTL_STRING;

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        LogMngr()
            throw()
        {
        }
    
        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~LogMngr()
            throw()
        {
        }

                        
        
        /**
         *Purpose:
         *<p>
         * Returns the identifier of the service.
         * Must be defined by a concrete logging manager.
         */
        virtual const char* getService() = 0;


        /**
         *Purpose:
         *<p>
         * Returns the specified message string or NULL if messageId is invalid.
         * Must be defined by a concrete logging manager.
         *
         * @param messageId the identifier of the message.
         */
        virtual const char* getMessage(long messageId) = 0;
        
        

        /**
         *Purpose:
         *<p>
         * Open the logging manager for using a log file.
         *
         *@param logFile the path of the log file.
         */
        static void open (const std::string& logFile)
        {
            if (logFile != "")
            {
            
                // We create a new file in append mode
                std::ofstream* pOut =
                    new std::ofstream(logFile.c_str(), std::ios::app);

                if (M_logStream != &std::cout)
                {
                    delete M_logStream;
                }
                
                M_logStream = pOut;
                
            }
        }
        


        /**
         *Purpose:
         *<p>
         * Close the log file used by the logging manager.
         * Must be called if open() has been called.
         */
        static void close ()
        {
            if (M_logStream != &std::cout)
            {
                delete M_logStream;
                M_logStream = &std::cout;
            }
        }
        
        
        /**
         *Purpose:
         *<p>
         * Destroys the logging manager.
         * Must be called if initialise() has been called.
         */
        static void destroy()
        {
            // close the log file if openned
            close ();
    
            // delete singleton
            if (M_pSingleton != NULL)
            {
                delete M_pSingleton;
                M_pSingleton = NULL;
            }
        }


        /**
        * Logs the specified message without any header.
        *
        * @param message the message.
        */
        static void logMessage(const char *message) throw()
        {
            doLogMessage(message);
        }


        /**
        * Logs the specified message.
        *
        * @param logType the type of log.
        * @param message the message.
        */
        static void logMessage(LogType logType, const char *message) throw()
        {

            try
            {
                std::string completeMessage = getHeader(logType);
                completeMessage += message;

                doLogMessage(completeMessage);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType, message);
            }

        }


        /**
        * Logs the specified message.
        *
        * @param logType the type of log.
        * @param messageId the identifier of the message.
        */
        static void logMessage(LogType logType, long messageId) throw()
        {

            try
            {
                std::string message = getHeader(logType);
                message += getFormattedMessage(messageId);

                doLogMessage(message);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType);
            }

        }


        /**
        * Logs the specified message containing a data.
        *
        * @param logType the type of log.
        * @param messageId the identifier of the message.
        * @param data1 the data that must be embedded in the message.
        */
        template <typename T1>
            static void logMessage(LogType logType, long messageId,
            T1 data1) throw()
        {

            try
            {
                std::string message = getHeader(logType);
                message += getFormattedMessage(messageId,
                    data1);

                doLogMessage(message);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType);
            }

        }


        /**
        * Logs the specified message containing two data.
        *
        * @param logType the type of log.
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        */
        template <typename T1, typename T2>
            static void logMessage(LogType logType, long messageId,
            T1 data1, T2 data2) throw ()
        {

            try
            {
                std::string message = getHeader(logType);
                message += getFormattedMessage(messageId,
                    data1, data2);

                doLogMessage(message);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType);
            }

        }


        /**
        * Logs the specified message containing three data.
        *
        * @param logType the type of log.
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        * @param data3 the third data that must be embedded in the message.
        */
        template <typename T1, typename T2, typename T3>
            static void logMessage(LogType logType, long messageId,
            T1 data1, T2 data2, T3 data3) throw()
        {

            try
            {
                std::string message = getHeader(logType);
                message += getFormattedMessage(messageId,
                    data1, data2, data3);

                doLogMessage(message);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType);
            }

        }


        /**
        * Logs the specified message containing four data.
        *
        * @param logType the type of log.
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        * @param data3 the third data that must be embedded in the message.
        * @param data4 the fourth data that must be embedded in the message.
        */
        template <typename T1, typename T2, typename T3, typename T4>
            static void logMessage(LogType logType, long messageId,
            T1 data1, T2 data2, T3 data3, T4 data4)
        {

            try
            {
                std::string message = getHeader(logType);
                message += getFormattedMessage(messageId,
                    data1, data2, data3, data4);

                doLogMessage(message);

            }
            catch(const std::bad_alloc&)
            {
                logOutOfMemory(logType);
            }

        }


        /**
        * Gets the specified formatted message.
        *
        * @param messageId the identifier of the message.
        */
        static std::string getFormattedMessage(long messageId)
        {

            std::string message = getSafeMessage(messageId);

            return message;

        }


        /**
        * Gets the specified formatted message containing a data.
        *
        * @param messageId the identifier of the message.
        * @param data1 the data that must be embedded in the message.
        */
        template <typename T1>
            static std::string getFormattedMessage(long messageId,
                T1 data1)
        {

            std::string message;

            std::string initialMessage = getSafeMessage(messageId);

            addMessagePart(message, initialMessage, data1);
            message += initialMessage;

            return message;

        }


        /**
        * Gets the specified formatted message containing two data.
        *
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        */
        template <typename T1, typename T2>
            static std::string getFormattedMessage(long messageId,
            T1 data1, T2 data2) throw ()
        {

            std::string message;

            std::string initialMessage = getSafeMessage(messageId);

            addMessagePart(message, initialMessage, data1);
            addMessagePart(message, initialMessage, data2);
            message += initialMessage;

            return message;

        }


        /**
        * Gets the specified formatted message containing three data.
        *
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        * @param data3 the third data that must be embedded in the message.
        */
        template <typename T1, typename T2, typename T3>
            static std::string getFormattedMessage(long messageId,
            T1 data1, T2 data2, T3 data3) throw()
        {

            std::string message;

            std::string initialMessage = getSafeMessage(messageId);

            addMessagePart(message, initialMessage, data1);
            addMessagePart(message, initialMessage, data2);
            addMessagePart(message, initialMessage, data3);
            message += initialMessage;

            return message;

        }


        /**
        * Gets the specified formatted message containing four data.
        *
        * @param messageId the identifier of the message.
        * @param data1 the first data that must be embedded in the message.
        * @param data2 the second data that must be embedded in the message.
        * @param data3 the third data that must be embedded in the message.
        * @param data4 the fourth data that must be embedded in the message.
        */
        template <typename T1, typename T2, typename T3, typename T4>
            static std::string getFormattedMessage(long messageId,
            T1 data1, T2 data2, T3 data3, T4 data4)
        {

            std::string message;

            std::string initialMessage = getSafeMessage(messageId);

            addMessagePart(message, initialMessage, data1);
            addMessagePart(message, initialMessage, data2);
            addMessagePart(message, initialMessage, data3);
            addMessagePart(message, initialMessage, data4);
            message += initialMessage;

            return message;

        }
    
       
    protected:
            
        /**
         *Purpose:
         *<p>
         * The LogMngr singleton pointer.
         */        
        static LogMngr* M_pSingleton; 
        

    private:

        /**
         *Purpose:
         *<p>
         * The log stream.
         * Points to std::cout by default.
         */
        static std::ostream* M_logStream;


        /**
         *Purpose:
         *<p>
         * The mutex used to protect concurrent access to the log stream.
         */
        static OsSupport::Mutex M_mutex;


        


        /**
         *Purpose:
         *<p>
         * Returns the string corresponding to the log type.
         *
         * @param logType the type of log.
         */
        static const char* getLogTypeString(LogType logType)
        {

            const char *logTypeString = NULL;

            switch (logType)
            {
                case WRN:
                    logTypeString = WRN_STRING;
                    break;

                case INF:
                    logTypeString = INF_STRING;
                    break;

                case ERR:
                    logTypeString = ERR_STRING;
                    break;

                case FTL:
                    logTypeString = FTL_STRING;
                    break;

            }

            return logTypeString;

        }


        /**
         *Purpose:
         *<p>
         * Returns the specified message string or an alert message
         * if messageId is invalid.
         *
         * @param messageId the identifier of the message.
         */
        static std::string getSafeMessage(long messageId)
        {

            std::string safeMessage;

            const char *message = M_pSingleton->getMessage(messageId);

            if (message != NULL)
            {
                safeMessage = message;
            }
            else
            {
                std::ostringstream temp;
                temp << "<<< Invalid message id (" << messageId << ") >>>" << std::ends;

                safeMessage = temp.str();
            }

            return safeMessage;

        }


        /**
         *Purpose:
         *<p>
         * Returns the header of a log message.
         *
         * @param logType the type of log.
         */
        static std::string getHeader(LogType logType)
        {

            const char *logTypeString = getLogTypeString(logType);

            time_t ltime;
            if (time(&ltime) == time_t(-1))
            {
                return "NoTime";    
            }
            
            char buffer[1024];

            /* Use time structure to build a customized time string. */
            tm now;
            ::localtime_r( &ltime, &now );

            strftime(buffer, 1024, "%Y/%m/%d %H:%M:%S", &now);
            std::string header = buffer;
            header += " ";
            header += logTypeString;
            header += " ";
            header += M_pSingleton->getService();
            header += " ";
            header += OsSupport::OS::get_hostname();
            header += " ";
            header += ":";
            
            return header;
        }	    



        /**
        * Logs actually the message.
        */
        static void doLogMessage(const std::string& message)
        {

            CDMW_MUTEX_GUARD(M_mutex);

            try
            {
                *LogMngr::M_logStream << message << std::endl << std::flush;
            }
            catch(...)
            {

            }
            
        }

        /**
        * Logs the lack of memory with a generic message.
        */
        static void logOutOfMemory(LogType logType) throw()
        {

            const char *logTypeString = getLogTypeString(logType);

            try
            {
                *M_logStream << std::endl << logTypeString
                    << ":<<< No memory for proper log message >>>"
                    << std::endl;
            }
            catch(...)
            {

            }
        }

        /**
        * Logs the lack of memory with a specific message.
        */
        static void logOutOfMemory(LogType logType, const char* message) throw()
        {

            const char *logTypeString = getLogTypeString(logType);

            try
            {
                *M_logStream << std::endl << logTypeString
                    << ":/" << message << "/" << std::endl;
            }
            catch(...)
            {

            }
        }

        /**
        * Adds a message part embedding the specified data to the message.
        * The position of the data is specified by a % in the initial message.
        *
        * @param message the message that will receive the message part.
        * @param initialMessage the message containing the position of the data.
        * The message part is removed from initialMessage. 
        * @param data the data to embed in the message part.
        */
        template <typename T>
            static void addMessagePart(
                std::string& message,
                std::string& initialMessage, T data)
        {
            std::string::size_type pos = initialMessage.find('%');

            if (pos == std::string::npos )
            {
                // ill-formed message
                message += initialMessage;
                initialMessage.erase();
            }
            else
            {

                message += initialMessage.substr(0,pos);
                initialMessage.erase(0, pos+2);

                std::ostringstream temp;
                temp << data;

                message += temp.str();

            }
        }


}; // End class LogMngr 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_LOGMNGR_HPP


