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


#include "platformadmin/GraphParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * Graph Successor Node Parser definition.
 */
GraphNodeParser::GraphSuccessorNodeParser::GraphSuccessorNodeParser(
    const DOM_Node& graphSuccessorNodeParser )
: ConfigElementParser( graphSuccessorNodeParser )
{
}

std::string GraphNodeParser::GraphSuccessorNodeParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string GraphNodeParser::GraphSuccessorNodeParser::host() const
{
    return get_attribute_value( "Host" );
}

unsigned short GraphNodeParser::GraphSuccessorNodeParser::step() const
{
    std::string step = get_attribute_value( "Step" );
    for( unsigned int i=0; i<step.length(); i++ )
    {
        if( (step[i] < '0') || (step[i]>'9') )
            CDMW_THROW1(
                ConfigException,
                std::string( "Step in the graph must be a number" ) );
    }
    return atol( step.c_str() );
}

/**
 * Graph Node Parser definition.
 */
GraphNodeParser::GraphNodeParser(
    const DOM_Node& graphNodeParser )
: ConfigElementParser( graphNodeParser )
{
}

std::string GraphNodeParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string GraphNodeParser::host() const
{
    return get_attribute_value( "Host" );
}

unsigned short GraphNodeParser::step() const
{
    std::string step = get_attribute_value( "Step" );
    for( unsigned int i=0; i<step.length(); i++ )
    {
        if( (step[i] < '0') || (step[i]>'9') )
            CDMW_THROW1(
                ConfigException,
                std::string( "Step in the graph must be a number" ) );
    }
    return atol( step.c_str() );
}

unsigned int GraphNodeParser::get_successor_node_count() const
{
    return get_child_count( "Graph-element-successor" );
}

std::string GraphNodeParser::get_successor_node_name(
    unsigned int successorNodeIndex ) const
{
    GraphSuccessorNodeParser graphSuccessorNodeParser(
        get_child_element( "Graph-element-successor", successorNodeIndex ) );

    return graphSuccessorNodeParser.name();
}

std::string GraphNodeParser::get_successor_node_host(
    unsigned int successorNodeIndex ) const
{
    GraphSuccessorNodeParser graphSuccessorNodeParser(
        get_child_element( "Graph-element-successor", successorNodeIndex ) );

    return graphSuccessorNodeParser.host();
}

unsigned short GraphNodeParser::get_successor_node_step(
    unsigned int successorNodeIndex ) const
{
    GraphSuccessorNodeParser graphSuccessorNodeParser(
        get_child_element( "Graph-element-successor", successorNodeIndex ) );

    return graphSuccessorNodeParser.step();
}

/**
 * Graph Root Node Parser definition.
 */
GraphParser::GraphRootNodeParser::GraphRootNodeParser(
    const DOM_Node& graphRootNodeParser )
: ConfigElementParser( graphRootNodeParser )
{
}

std::string GraphParser::GraphRootNodeParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string GraphParser::GraphRootNodeParser::host() const
{
    return get_attribute_value( "Host" );
}

unsigned short GraphParser::GraphRootNodeParser::step() const
{
    std::string step = get_attribute_value( "Step" );
    for( unsigned int i=0; i<step.length(); i++ )
    {
        if( (step[i] < '0') || (step[i]>'9') )
            CDMW_THROW1(
                ConfigException,
                std::string( "Step in the graph must be a number" ) );
    }
    return atol( step.c_str() );
}

/**
 * Graph definition.
 */
GraphParser::GraphParser(
    const DOM_Node& graphParser )
: ConfigElementParser( graphParser )
{
}

unsigned int GraphParser::get_graph_node_count() const
{
    return get_child_count( "Graph-element" );
}

GraphNodeParser GraphParser::get_graph_node(
    unsigned int graphNodeIndex ) const
{
    return GraphNodeParser(
        get_child_element( "Graph-element", graphNodeIndex ) );
}

unsigned int GraphParser::get_root_node_count() const
{
    return get_child_count( "Graph-element-root" );
}

std::string GraphParser::get_root_node_name(
    unsigned int rootNodeIndex ) const
{
    GraphRootNodeParser graphRootNodeParser(
        get_child_element( "Graph-element-root", rootNodeIndex ) );

    return graphRootNodeParser.name();
}

std::string GraphParser::get_root_node_host(
    unsigned int rootNodeIndex ) const
{
    GraphRootNodeParser graphRootNodeParser(
        get_child_element( "Graph-element-root", rootNodeIndex ) );

    return graphRootNodeParser.host();
}

unsigned short GraphParser::get_root_node_step(
    unsigned int rootNodeIndex ) const
{
    GraphRootNodeParser graphRootNodeParser(
        get_child_element( "Graph-element-root", rootNodeIndex ) );

    return graphRootNodeParser.step();
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

