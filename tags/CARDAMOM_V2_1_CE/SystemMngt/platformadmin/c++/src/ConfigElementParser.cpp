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


#include "platformadmin/ConfigElementParser.hpp"
#include <iostream>

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

std::string to_string(
    const DOMString& s )
{
    char* c_str = s.transcode();
    std::string str( c_str );

    delete c_str;
    return str;
}

/**
 * Configuration Parser.
 */
ConfigElementParser::ConfigElementParser(
    const DOM_Node& element )
{
    m_element = (DOM_Element &) element;
}

DOM_Node ConfigElementParser::get_parent_element() const
{
    if( m_element.getParentNode().getNodeType() != DOM_Node::ELEMENT_NODE )
        CDMW_THROW( NotFoundException );

    return m_element.getParentNode();
}

unsigned int ConfigElementParser::get_child_count(
    const char* elementTypeName ) const
{
    /**
     * DO NOT USE getElementsByTagName().
     *
     * The method getElementsByTagName() returns all the descendant
     * (children, grand-children, etc,...) nodes under the specified one.
     * Use getChildNodes() instead.
     */
    DOMString    typeName       = DOMString::transcode( elementTypeName );
    DOM_NodeList childNodeList  = m_element.getChildNodes();
    unsigned int childNodeCount = childNodeList.getLength();
    unsigned int foundNodeCount = 0;
    
    while( childNodeCount > 0 )
    {
        if( childNodeList.item( --childNodeCount ).getNodeName().equals( typeName ) )
            foundNodeCount++;
    }
    
    return foundNodeCount;
}

DOM_Node ConfigElementParser::get_child_element(
    const char* elementTypeName,
    unsigned int elementIndex ) const
{
    /**
     * DO NOT USE getElementsByTagName().
     *
     * The method getElementsByTagName() returns all the descendant
     * (children, grand-children, etc,...) nodes under the specified one.
     * Use getChildNodes() instead.
     */
    DOMString    typeName       = DOMString::transcode( elementTypeName );
    DOM_NodeList nodeList       = m_element.getChildNodes();
    unsigned int nodeCount      = nodeList.getLength();
    unsigned int childNodeIndex = 0;
    unsigned int foundNodeIndex = 0;
    
    while( (childNodeIndex < nodeCount) && (foundNodeIndex <= elementIndex) )
    {
        if( nodeList.item( childNodeIndex++ ).getNodeName().equals( typeName ) )
            foundNodeIndex++;
    }
    if( foundNodeIndex <= elementIndex )
        CDMW_THROW( NotFoundException );
    
    return nodeList.item( childNodeIndex-1 );
}

std::string ConfigElementParser::get_attribute_value(
    const char* attributeName ) const
{
    std::string attribute_value;

    attribute_value = to_string(
        m_element.getAttribute(
            DOMString::transcode( attributeName ) ) );

    return attribute_value;
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

