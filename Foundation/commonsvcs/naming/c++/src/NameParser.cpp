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


#include "naming/NameParser.hpp"
#include "naming/Cdmw_NamingInterface_NameParserFSM.hpp"
#include "Foundation/common/Assert.hpp"

#include <cstdio>

// The c++ Standard Library GNU with 2.95, provides only
//  the deprecated strstream
#    include <sstream>


namespace
{
    // TODO: [i18n]put string in a message catalog
    const char* INVALID_SEQUENCE = "The empty sequence is not a legal name";
    const char* NO_MEMORY_FOR_NAME = "Cannot allocate memory for target name";
    const char* PARSER_ERROR = "Name Parser internal error (failed to allocate memory)";
};

namespace Cdmw
{
namespace CommonSvcs
{

    namespace Naming
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        NameParser::NameParser()
        throw()
        {
            // does nothing
        }


        NameParser::~NameParser()
        throw()
        {
            // does nothing
        }


        NameParser::NameParser(const NameParser& rhs)
        throw()
        {
            // nothing to copy
        }

        NameParser&
        NameParser::operator=(const NameParser& rhs)
        throw()
        {
            // nothing to assing
            return * this;
        }

        CosNaming::Name*
        NameParser::to_name(const std::string & strn) throw (InvalidNameException)
        {
            try
            {
                Cdmw_NamingInterface_NameParserFSM parser;

                std::istringstream is (strn);

                parser.set_input_data(is);

                bool finished = false;

                while (!finished)
                {
                    switch (parser.ReadChar())
                    {

                            case EOF:
                            parser.eos();
                            finished = true;
                            break;

                            case '/':
                            parser.slash();
                            break;

                            case '.':
                            parser.dot();
                            break;

                            case '\\':
                            parser.back_slash();
                            break;

                            default:
                            parser.other();
                            break;
                    }
                }

                return parser.get_name();
            }
            catch (const OutOfMemoryException &)
            {
                CDMW_THROW1(InvalidNameException, PARSER_ERROR);
            }
            catch (...)
            { // More likely to be an std::bad_alloc exception!
                CDMW_THROW1(InvalidNameException, PARSER_ERROR);
            }
        }

        std::string
        NameParser::to_string(const CosNaming::Name& n)
        throw (InvalidNameException)
        {
            if (n.length() == 0)
                CDMW_THROW1(InvalidNameException, INVALID_SEQUENCE);

            try
            {
                std::string s;

                for (CORBA::ULong i = 0 ; i < n.length() ; i++)
                {
                    if (i != 0)
                        s += "/";

                    s += NameComponent_to_string(n[i]);
                }

                return s;
            }
            catch (const std::bad_alloc & )
            {
                CDMW_THROW1(InvalidNameException, NO_MEMORY_FOR_NAME);
            }
        }

        //
        // INS compatible stringifying of a CosNaming::NameComponent.
        //
        // The single '.' character is the only representation of a name
        // component with empty id and kind fields.
        // If a name component has a non-empty id field and an empty kind field,
        // the stringified representation consists only of the id field. A trailing
        // '.' character is not permitted.
        std::string
        NameParser::NameComponent_to_string(const CosNaming::NameComponent& nc)
        throw (std::bad_alloc)
        {
            std::string s = escape_field (nc.id.in());

            if ((nc.kind[0] != '\0') ||
                    (nc.id[0] == '\0'))
            {
                s += ".";
                s += escape_field (nc.kind.in());
            }

            return s;
        }

        //
        // Add INS stringified-name escapes.
        // Characters '.', '/' and '\\' must be escaped with a '\\'.
        //
        std::string
        NameParser::escape_field(const char* s) throw(std::bad_alloc)
        {
            CDMW_ASSERT(s != NULL);
            std::string escaped;

            while (*s)
            {
                switch (*s)
                {

                        case '/':

                        case '.':

                        case '\\':
                        escaped += '\\';

                        default:
                        escaped += *s;
                }

                s++;
            }

            return escaped;
        }



    } // End namespace Naming

} // End namespace CommonSvcs
} // End namespace Cdmw



