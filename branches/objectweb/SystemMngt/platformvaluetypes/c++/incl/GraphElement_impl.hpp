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


#ifndef INCL_PLATFORMMNGT_GRAPH_ELEMENT_HPP
#define INCL_PLATFORMMNGT_GRAPH_ELEMENT_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "idllib/CdmwPlatformMngtCommon.stub.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::GraphElement objects.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class GraphElementFactory : public virtual CdmwPlatformMngt::GraphElement_init
        {

            private:
                /**
                 * Purpose:
                 * <p> Inherited method used for unmarshalling factory object.
                 * 
                 *@return The unmarshalled factory.
                 */
                virtual CORBA::ValueBase* create_for_unmarshal();

            public:
                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::GraphElement object with the given properties
                 * provided as parameters.
                 * 
                 *@param name The name of the CdmwPlatformMngt::GraphElement object.
                 *@param type The index of the graph element's step.
                 *
                 *@return the newly created CdmwPlatformMngt::GraphElement object.
                 */
                virtual CdmwPlatformMngt::GraphElement* create(
                    const char* elementName,
                    const unsigned short stepIndex );
        };

        /**
         *Purpose:
         *<p> An implementation class.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class GraphElement_impl : public virtual OBV_CdmwPlatformMngt::GraphElement,
                    public virtual CORBA::DefaultValueRefCountBase
        {

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                GraphElement_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::GraphElement_impl object with the given properties
                 * provided as parameters.
                 * 
                 *@param name The name of the graph element object.
                 *@param type The index of the graph element's step.
                 */
                GraphElement_impl(
                    const char* elementName,
                    const unsigned short stepIndex );

                friend class GraphElementFactory;

            public:
                /**
                 * Inherited from CORBA::ValueBase
                 */
                virtual CORBA::ValueBase* _copy_value();
        };

    } // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_GRAPH_ELEMENT_HPP

