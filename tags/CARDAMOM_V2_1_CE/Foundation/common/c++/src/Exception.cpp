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


#include <sstream>

#include "Foundation/common/Exception.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    //
    // TODO: if we have string into the message, we are not able to propagate
    //    an exception if no more memory is available ??
    // Maybe a specific allocator, or an internal buffer ( but what in case of
    //    overflow ?)

    Exception::Exception (Severity severity, std::string info)
            : m_severity(severity),
            m_info(info),
            m_file(""),
            m_fullInfo(info)
    {}

    Exception::Exception( const Exception& rhs)
            : m_severity(rhs.m_severity),
            m_info(rhs.m_info),
            m_file(rhs.m_file),
            m_line(rhs.m_line),
            m_fullInfo(rhs.m_fullInfo)
    {
        // Nothing to do
    }

    Exception::~Exception() throw()
    {}

    const char* Exception::what() const throw()
    {
        return m_fullInfo.c_str();
    }

    const std::string& Exception::get_info() const
    {
        return m_info;
    }

    const std::string& Exception::get_file() const
    {
        return m_file;
    }

    void Exception::set_file(const std::string& file)
    {
        m_file = file;
    }

    int Exception::get_line() const
    {
        return m_line;
    }

    void Exception::set_line(int line)
    {
        m_line = line;

        m_fullInfo = "File:";
        m_fullInfo += m_file;
        m_fullInfo += " Line:";

        std::ostringstream line_str;
        line_str << m_line;
        m_fullInfo += line_str.str();
        m_fullInfo += " Reason:";

        m_fullInfo += m_info;
    }




    OutOfMemoryException::OutOfMemoryException()
            : Exception( Exception::SAFE, "Out of memory" )
    {}

    OutOfResourcesException::OutOfResourcesException()
            : Exception( Exception::SAFE, "Out of resources" )
    {}

    InternalErrorException::InternalErrorException(const std::string& parameter)
            : Exception( Exception::FATAL, "Internal error" + parameter)
    {}

    AlreadyDoneException::AlreadyDoneException()
            : Exception( Exception::SAFE, "Action already done" )
    {}

    AssertionFailedException::AssertionFailedException()
            : Exception( Exception::FATAL, "Assertion failed" )
    {}

    PermissionDeniedException::PermissionDeniedException()
            : Exception( Exception::SAFE, "Permission denied" )
    {}

    BadParameterException::BadParameterException(
        const std::string& parameterName,
        const std::string& parameterValue)

            : Exception( Exception::SAFE, "Parameter invalid" ),
            m_parameterName(parameterName),
            m_parameterValue(parameterValue)
    {
        m_what = std::string(Exception::what()) + " (" + m_parameterName + "=" + m_parameterValue + ")";
    }

    BadParameterException::~BadParameterException()
    throw()
    {}


    const char* BadParameterException::what() const throw()
    {
        return m_what.c_str();
    }

    InitErrorException::InitErrorException( const std::string& reason,
                                            Exception::Severity severity)
        : Exception( severity, "Initialisation Error : " + reason )
    {
    }
    
    NotExistException::NotExistException()
            : Exception( Exception::SAFE, "Reference does not exist" )
    {}
};



std::ostream& operator<< (std::ostream& os, const Cdmw::Exception& ex)
{
    os << "Cdmw Exception :" << std::endl;
    os << " File   : " << ex.get_file() << std::endl;
    os << " Line   : " << ex.get_line() << std::endl;
    os << " Reason : " << ex.get_info() << std::endl;

    return os;
}





