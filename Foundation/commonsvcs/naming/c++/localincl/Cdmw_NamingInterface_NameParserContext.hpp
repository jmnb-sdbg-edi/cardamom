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


#ifndef INCL_COMMONSVCS_NAMING_NAMEPARSERCONTEXT_HPP
#define INCL_COMMONSVCS_NAMING_NAMEPARSERCONTEXT_HPP

#include "Foundation/commonsvcs/naming/InvalidNameException.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"

#include <string>
#include <iostream>


/**
*Purpose:
*<p> This is the context class of the corba name parser.
* This class knows about the input stream,
* How to read and write chars, and which critical chars to
* look for in the input stream.
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/

class Cdmw_NamingInterface_NameParserContext
{

    public:

        /**
        * Purpose:
        * <p> Default Constructor
        * 
        *@exception Cdmw::OutOfMemoryException Failure to allocate memory
        */
        Cdmw_NamingInterface_NameParserContext()
        throw(Cdmw::OutOfMemoryException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */
        virtual
        ~Cdmw_NamingInterface_NameParserContext()
        throw();


        /**
        * Purpose:
        * <p> Sets the input stream containg the stringified name for parsing.
        * <B>WARNING:</B> This should be called before any other operation on a 
        * NameParserContext object
        * 
        *@param is A reference to the input stream
        */
        inline void set_input_data(std::istream & is)
        {
            m_istream = & is;
        }

        /**
        * Purpose:
        * <p> Diagnostic function to be called by the state machine whenever there
        * is a transition error.
        *@param t event starting the transition
        *@param s current state within the state machine
        *@exception Cdmw::CommonSvcs::Naming::InvalidNameException Contains an error message
        */
        void FSMError(const char* t, const char* s)
        throw (Cdmw::CommonSvcs::Naming::InvalidNameException);

        /**
        * Purpose:
        * <p> Extracts a character from the input stream if available
        * 
        *@return Returns extracted character if available otherwise returns EOF .
        */
        inline int ReadChar() throw ()
        {
            int c = EOF;

            try
            {

                c = m_istream->get
                    ();
            }
            catch (...)
            {
                // Exception safety oblige!
                // consume exception.
            }

            m_char = c;

            return c;
        }

        /**
        * Purpose:
        * <p> Returns the translation of the internal input stream to a CosNaming::Name build
        * and transfers ownership of the name to the caller.
        * <B>NOTA:</B> This can only be done once because of the transfer of ownership
        *@return A pointer to the result of the parsing of the input stream
        */
        inline CosNaming::Name* get_name()
        {
            return m_name._retn();
        };

        // Actions
        /**
        * Purpose:
        * <p> Adds/Appends currently parsed id and kind fields to the internal
        * CosNaming::Name used as a taget for the current parsing.
        * 
        */
        void add_name_component();

        /**
        * Purpose:
        * <p> Adds/Appends currently parsed id field as a last CosNaming::NameComponent
        * to the internal CosNaming::Name used as a taget for the current parsing.
        * 
        */
        void add_last_id();

        /**
        * Purpose:
        * <p> Adds/Appends currently parsed id and kind fields as a last
        * CosNaming::NameComponent to the internal CosNaming::Name used as a taget
        * for the current parsing.
        * 
        *@exception Cdmw::CommonSvcs::Naming::InvalidNameException A trailing
        * '.' is used but this is not permitted by INS specification.
        */
        void add_last_kind() throw (Cdmw::CommonSvcs::Naming::InvalidNameException);

        /**
        * Purpose:
        * <p> Checks that currently parsed id field is not empty when no '.'
        * is present in the last stringified name component; as a single '.'
        * character is the only representation of a name component with empty id and
        * kind fields.
        * 
        *@exception Cdmw::CommonSvcs::Naming::InvalidNameException Invalid empty name component
        * (a '.' may be missing)
        */
        void check_id_not_empty() throw (Cdmw::CommonSvcs::Naming::InvalidNameException);

        /**
        * Purpose:
        * <p> Appends last extracted character to the id field in progress
        * 
        */
        inline void put_char_in_id()
        {
            m_id += m_char;
        }

        /**
        * Purpose:
        * <p> Appends last extracted character to the kind field in progress
        *
        */
        inline void put_char_in_kind()
        {
            m_kind += m_char;
        }

        /**
        * Purpose:
        * <p> Called for unexpected end of string
        * 
        *@exception Cdmw::CommonSvcs::Naming::InvalidNameException Unexpected end of
        * string. Raised when the input string ends with an escaped character is
        * missing.
        */
        void unexpected_eos() throw (Cdmw::CommonSvcs::Naming::InvalidNameException);


        /**
        *[Attribute description]
        */


    protected:


    private:
        /**
        * Holds the last extracted char from the input stream
        */
        char m_char;
        /**
        * A pointer to the input stream holding the stringified name for parsing.
        */
        std::istream * m_istream;
        /**
        * Current Id field in the parsing process
        */
        std::string m_id;
        /**
        * Current Kind field in the parsing process
        */
        std::string m_kind;

        /**
        * Internal CosNaming::Name for holding the parsed stringified name.
        */
        CosNaming::Name_var m_name;

        /**
        * Number of CosNaming::NameComponent in the CosNaming::Name above.
        */
        CORBA::ULong m_components;

        /**
        * Purpose:
        * <p> Append a CosNaming::NameComponent to the the internal CosNaming::Name
        * using provided id and kind values.
        * It resets m_id and m_kind content to ""
        * 
        *@param id_value Value of the <I>id</I> field
        *@return kind_value Value of the <I>kind</I> field
        *@exception
        */
        void append_name_component(const char* id_value,
                                   const char* kind_value);
        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */
        Cdmw_NamingInterface_NameParserContext(const Cdmw_NamingInterface_NameParserContext& rhs)
        throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */
        Cdmw_NamingInterface_NameParserContext&
        operator=(const Cdmw_NamingInterface_NameParserContext& rhs)
        throw();

}; // End class NameParserContext

#endif // INCL_COMMONSVCS_NAMING_NAMEPARSERCONTEXT_HPP

