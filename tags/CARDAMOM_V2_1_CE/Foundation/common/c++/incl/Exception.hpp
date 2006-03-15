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


#ifndef CDMW_EXCEPTION_H
#define CDMW_EXCEPTION_H

#include <stdexcept>
#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{

    /**
    *Purpose:
    *<p>
    *Base class for Cdmw exceptions
    */

    class Exception : public std::exception
    {

        public:

            /**
            * Define values for Exception severity
            */
            enum Severity {
                SAFE,
                FATAL,
                UNKNOWN
            };

            /**
            * Construct the exception object
            *
            * @parameter severity : Severity of exception
            * @parameter info : exception reason
            */
            Exception (Severity severity, std::string info);

            //
            // Copy Ctor
            //
            Exception (const Exception& rhs);


            virtual ~Exception () throw();

            /**
            * Get full information about exception
            *
            * @return : the exception full information : 
            *           file, line , reason
            */
            virtual const char* what() const throw();

            /**
            * Get exception reason
            *
            * @return : the exception reason 
            */
            const std::string& get_info() const;

            /**
            * Get file where exception has been raised
            *
            * @return : the exception file 
            */
            const std::string& get_file() const;

            /**
            * Set file where exception has been raised
            *
            * @parameter : the exception file 
            */
            void set_file(const std::string& file);

            /**
            * Get line where exception has been raised
            *
            * @return : the exception line 
            */
            int get_line() const;

            /**
            * Set line where exception has been raised
            *
            * @parameter : the exception line 
            */
            void set_line(int line);


        protected:


        private:
            /**
            * severity of exception
            */
            Severity m_severity;

            /**
            * reason of exception
            */
            const std::string m_info;

            /**
            * file of exception
            */
            std::string m_file;

            /**
            * line of exception
            */
            int m_line;

            /**
            * full information on exception
            */
            std::string m_fullInfo;

            //
            // Ctor
            //
            Exception();
    };

    /**
    *Purpose:
    *<p>
    *Out Of Memory Exception
    */

    class OutOfMemoryException : public Exception
    {

        public:
            OutOfMemoryException();

        protected:

        private:

    };


    /**
    *Purpose:
    *<p>
    *Out Of Resource Exception.
    *Resource includes memory and persistent storage.
    */

    class OutOfResourcesException : public Exception
    {

        public:
            OutOfResourcesException();
    };


    /**
    *Purpose:
    *<p>
    *Internal Error Exception
    */

    class InternalErrorException : public Exception
    {

        public:
            InternalErrorException(const std::string& parameter = "");

        protected:

        private:

    };

    /**
    *Purpose:
    *<p>
    *Already Done Exception
    */

    class AlreadyDoneException : public Exception
    {

        public:
            AlreadyDoneException();

        protected:

        private:

    };

    /**
    *Purpose:
    *<p>
    *Bad Order Exception
    */

    class BadOrderException : public Exception
    {

        public:
            BadOrderException()
                    : Exception( Exception::SAFE, "Bad order" )
            {}

    }

    ;

    /**
    *Purpose:
    *<p>
    *Assertion Failed Exception
    */

    class AssertionFailedException : public Exception
    {

        public:
            AssertionFailedException();

        protected:

        private:

    };


    /**
    *Purpose:
    *<p>
    *Permission Denied Exception
    */

    class PermissionDeniedException : public Exception
    {

        public:
            PermissionDeniedException();

        protected:

        private:

    };

    /**
    *Purpose:
    *<p>
    *Bad Parameter Exception
    *
    * @param parameterName  the name of the conflicting parameter
    * @param parameterValue the value of this parameter
    */

    class BadParameterException : public Exception
    {

        public:
            BadParameterException(
                const std::string& parameterName,
                const std::string& parameterValue);

            virtual ~BadParameterException() throw();
            virtual const char* what() const throw();


            const std::string m_parameterName;
            const std::string m_parameterValue;

        private:

            std::string m_what;

    };


    /**
    *Purpose:
    *<p>
    *Bad Parameter Exception
    *
    * @param parameterName  the name of the conflicting parameter
    * @param parameterValue the value of this parameter
    */

    class InitErrorException : public Exception
    {
    public:
        
        /**
         * Construct the exception object
         *
         * @parameter reason : reason of the initialisation failure
         * @parameter severity : Severity of exception
         */        
        InitErrorException( const std::string& reason,
                            Exception::Severity severity = Exception::FATAL);
        
        ~InitErrorException() throw() {}
    };


    /**
    *Purpose:
    *<p>
    *Not Exist Exception
    */

    class NotExistException : public Exception
    {

        public:
            NotExistException();

        protected:

        private:

    };

};


/**
*Purpose:
*<p>
*Exception output on std::ostream
*/
std::ostream& operator<< (std::ostream&, const Cdmw::Exception&);


#define CDMW_THROW( ExType ) do { ExType ex; \
        ex.set_file(__FILE__); \
        ex.set_line(__LINE__); \
        throw ex; } while(0)

#define CDMW_THROW1( ExType, arg_1 ) do { ExType ex(arg_1) ; \
        ex.set_file(__FILE__); \
        ex.set_line(__LINE__); \
        throw ex; }  while(0)

#define CDMW_THROW2( ExType, arg_1, arg_2 ) do { ExType ex(arg_1, arg_2) ; \
        ex.set_file(__FILE__); \
        ex.set_line(__LINE__); \
        throw ex; }  while(0)

#define CDMW_THROW3( ExType, arg_1, arg_2, arg_3 ) do { ExType ex(arg_1, arg_2, arg_3) ; \
        ex.set_file(__FILE__); \
        ex.set_line(__LINE__); \
        throw ex; }  while(0)


#endif // CDMW_EXCEPTION_H

