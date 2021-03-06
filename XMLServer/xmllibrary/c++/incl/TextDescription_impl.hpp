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


#ifndef INCL_TEXTDESCRIPTION_IMPL_HPP
#define INCL_TEXTDESCRIPTION_IMPL_HPP

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW XMLUtils types and data.
    */

    namespace XMLUtils
    {

        class TextDescription_impl : public virtual OBV_CdmwXML::TextDescription,
                    public virtual CORBA::DefaultValueRefCountBase
        {

            protected:

                TextDescription_impl();
                TextDescription_impl(CdmwXML::NodeType type,
                                     CdmwXML::Node_ptr reference,
                                     const CdmwXML::DOMString& data);

                friend class TextDescriptionFactory_impl;

            public:

                //
                // Inherited from CORBA::ValueBase
                //
                virtual CORBA::ValueBase* _copy_value();

                //
                // IDL:thalesgroup.com/CdmwXML/NodeDescription/type:1.0
                //
                virtual CdmwXML::NodeType type();

                //
                // IDL:thalesgroup.com/CdmwXML/NodeDescription/reference:1.0
                //
                virtual CdmwXML::Node_ptr reference();

                //
                // IDL:thalesgroup.com/CdmwXML/TextDescription/data:1.0
                //
                virtual CdmwXML::DOMString* data();
                virtual void data(const CdmwXML::DOMString& newData);

        };


    }

    ; // End namespace XMLUtils
}

; // End namespace Cdmw
#endif

