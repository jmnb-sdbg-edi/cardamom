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


#ifndef INCL_NAMINGANDREPOSITORY_NAMEPARSER_HPP
#define INCL_NAMINGANDREPOSITORY_NAMEPARSER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"

#include "Repository/naminginterface/InvalidNameException.hpp"

#include <new> // for std::bad_alloc

namespace Cdmw
{

    namespace NamingAndRepository
    {

        /**
        *Purpose:
        *<p>    [class description]
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class NameParser
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                NameParser()
                throw();

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                NameParser(const NameParser& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~NameParser()
                throw();


                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                NameParser&
                operator=(const NameParser& rhs)
                throw();



                /**
                * Purpose:
                * <p>This operation accepts a stringified name and returns a CosNaming::Name.
                * 
                *@param sn A stringified name, INS compatible, to be converted to a CosNaming::Name.
                *
                *@return A pointer to a CosNaming::Name that holds the decoded string.
                *
                *@exception InvalidNameException Indicates that the input string is syntactically
                * malformed.
                */
                static CosNaming::Name* to_name(const std::string & sn)
                throw (InvalidNameException);


                /**
                * Purpose:
                * <p> This operation accepts a CosNaming::Name and returns a stringified
                * name.
                * 
                *@param name A CosNaming::Name to stringify.
                *
                *@return A stringified name, INS compatible, of the input CosNaming::Name.
                *
                *@exception InvalidNameException Indicates that the name is invalid (length 0)
                */
                static std::string to_string(const CosNaming::Name& n)
                throw (InvalidNameException);


                /**
                *[Attribute description]
                */


            protected:


            private:
                /**
                * Purpose:
                * <p> Add INS stringified-name escapes to a CosNaming::NameComponent's field.
                * Characters '.', '/' and '\\' will be escaped with a '\\'.
                * 
                *@param s A CosNaming::NameComponent's field (id or kind)
                *@return A properly escaped field
                *@exception std::bad_alloc Failure to allocate memory for the output string.
                */
                static std::string escape_field(const char* s)
                throw(std::bad_alloc);

                /**
                * Purpose:
                * <p> INS compatible stringifying of a CosNaming::NameComponent.
                * <B>NOTA:</B>
                * <UL>
                * <LI> The single '.' character is the only representation of a name 
                *  component with empty id and kind fields.
                * <LI> If a name component has a non-empty id field and an empty kind field, 
                *  the stringified representation consists only of the id field. A trailing 
                *  '.' character is not permitted.
                * </UL>
                * 
                *@param nc A CosNaming::NameComponent to be stringified
                *@return A properly stringified name component
                *@exception std::bad_alloc Failure to allocate memory for the output string.
                */
                static std::string NameComponent_to_string(const CosNaming::NameComponent& nc)
                throw (std::bad_alloc);

        }

        ; // End class NameParser

    } // End namespace NamingAndRepository
} // End namespace Cdmw

#endif // INCL_NAMINGANDREPOSITORY_NAMEPARSER_HPP

