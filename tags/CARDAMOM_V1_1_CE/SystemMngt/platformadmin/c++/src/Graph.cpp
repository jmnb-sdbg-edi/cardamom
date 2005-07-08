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


#include "platformadmin/Graph.hpp"
#include "platformadmin/GraphParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/SystemParser.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"

#include <sstream>

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

// ----------------------------------------------------------------------
// Graph class.
// ----------------------------------------------------------------------
CdmwPlatformMngt::GraphElement* Graph::get_graph_element(
    std::string name,
    unsigned short step )
{
    try
    {
        CdmwPlatformMngt::GraphElement_var graphElement;

        // Search the specified graph element
        std::ostringstream key;

        key << name << "-" << step << std::ends;
        PlatformMngt::GraphElementMap::iterator iterator
            = m_graphElementMap.find( key.str() );

        if( iterator != m_graphElementMap.end() )
        {
            // Found element, get its reference
            graphElement = iterator->second;
        }
        else
        {
            // Not found element, create a new reference
            #if CDMW_ORB_VDR == tao
            Cdmw::PlatformMngt::GraphElementFactory graphElementFactory;

            graphElement = graphElementFactory.create( name.c_str(), step );
            #else
            CORBA::ValueFactoryBase_var graphElementFactory
                = new Cdmw::PlatformMngt::GraphElementFactory();

            graphElement
                = dynamic_cast< Cdmw::PlatformMngt::GraphElementFactory* >
                    ( graphElementFactory.in() )->create( name.c_str(), step );
            #endif

            // Add it to the map of graph element references
            m_graphElementMap.insert(
                PlatformMngt::GraphElementMap::value_type(
                    key.str(),
                    graphElement ) );
        }

        // return the graph element's reference
        return graphElement._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

}

Graph::Graph(
    const GraphParser& graphParser )
{
    try
    {
        // Load the dependency graph
        for( unsigned int nodeIndex=0;
             nodeIndex < graphParser.get_graph_node_count();
             nodeIndex++ )
        {
            // Get the reference of each graph element
            GraphNodeParser nodeParser
                = graphParser.get_graph_node( nodeIndex );
            CdmwPlatformMngt::GraphElement_var graphElement
                = get_graph_element(
                    nodeParser.name(),
                    nodeParser.step() );

            // Assign the successors's reference
            unsigned int successorNodeCount
                = nodeParser.get_successor_node_count();

            graphElement->successors().length( successorNodeCount );
            for( unsigned int successorIndex=0;
                 successorIndex < successorNodeCount;
                 successorIndex++ )
            {
                CdmwPlatformMngt::GraphElement_var successorGraphElement
                    = get_graph_element(
                        nodeParser.get_successor_node_name( successorIndex ),
                        nodeParser.get_successor_node_step( successorIndex ) );

                graphElement->successors()[ successorIndex ]
                    = successorGraphElement;
            }
        }

        // Determine the root elements of the graph
        unsigned int rootNodeCount = graphParser.get_root_node_count();

        m_roots = new CdmwPlatformMngt::GraphElements();
        m_roots->length( rootNodeCount );
        for( unsigned int rootNodeIndex = 0;
             rootNodeIndex < rootNodeCount;
             rootNodeIndex++ )
        {
            // Get the reference of each root element of the graph
            CdmwPlatformMngt::GraphElement_var root
                = get_graph_element(
                    graphParser.get_root_node_name( rootNodeIndex ),
                    graphParser.get_root_node_step( rootNodeIndex ) );

            // Store it into the internal list
            m_roots[ rootNodeIndex ] = root;
        }
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

Graph::~Graph()
{
}

CdmwPlatformMngt::GraphElements* Graph::get_roots() const
{
    try
    {
        CdmwPlatformMngt::GraphElements_var roots
            = new CdmwPlatformMngt::GraphElements( m_roots.in() );

        return roots._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

// ----------------------------------------------------------------------
// ApplicationGraph class.
// ----------------------------------------------------------------------
ApplicationGraph::ApplicationGraph(
    const GraphParser& graphParser )
: Graph( graphParser )
{
    PlatformMngt::GraphChecker applicationGraphChecker(
        m_roots.in(), NULL, true );
}

ApplicationGraph::ApplicationGraph(
    const SystemParser& systemParser,
    const GraphParser& graphParser )
: Graph( graphParser )
{
    try
    {
        // Build the map of element's name as defined in the system definition
        for( unsigned int applicationIndex=0;
             applicationIndex < systemParser.get_application_count();
             applicationIndex++ )
        {
            ApplicationParser applicationParser
                = systemParser.get_application( applicationIndex );

            if( m_existingElements.find( applicationParser.name().c_str() ) ==
                m_existingElements.end() )
            {
                m_existingElements.insert(
                    PlatformMngt::ExistingElementMap::value_type(
                        applicationParser.name().c_str(), 0 ));
            }
        }

        // Check all the graph elements are defined
        // in the definition of the system
        PlatformMngt::GraphElementMap::iterator graphElementIt;
        
        for( graphElementIt = m_graphElementMap.begin();
             graphElementIt != m_graphElementMap.end();
             graphElementIt++ )
        {
            PlatformMngt::ExistingElementMap::iterator foundElement
                = m_existingElements.find( graphElementIt->second->name() );
            if( foundElement == m_existingElements.end() )
                throw CdmwPlatformMngt::ReferenceNotFound(
                    graphElementIt->second->name() );
        }

        // Do more verifications on the graph
        PlatformMngt::GraphChecker applicationGraphChecker(
            m_roots.in(), &m_existingElements, true );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

// ----------------------------------------------------------------------
// ProcessGraph class.
// ----------------------------------------------------------------------
ProcessGraph::ProcessGraph(
    const GraphParser& graphParser,
    bool ignoreStep )
: Graph( graphParser )
{
    PlatformMngt::GraphChecker processGraphChecker(
        m_roots.in(), NULL, ignoreStep );
}

ProcessGraph::ProcessGraph(
    const ApplicationParser& applicationParser,
    const GraphParser& graphParser,
    bool ignoreStep )
: Graph( graphParser )
{
    try
    {
        // Build the map of processes' name and number of steps as
        // defined in the application
        for( unsigned int managedProcessIndex = 0;
             managedProcessIndex < applicationParser.get_managed_process_count();
             managedProcessIndex++ )
        {
            ProcessParser processParser
                = applicationParser.get_managed_process( managedProcessIndex );

            if( m_existingElements.find( processParser.name().c_str() ) ==
                m_existingElements.end() )
            {
                m_existingElements.insert(
                    PlatformMngt::ExistingElementMap::value_type(
                        processParser.name().c_str(),
                        processParser.get_init_step_count() ) );
            }
        }

        for( unsigned int unmanagedProcessIndex = 0;
             unmanagedProcessIndex < applicationParser.get_unmanaged_process_count();
             unmanagedProcessIndex++ )
        {
            ProcessParser processParser
                = applicationParser.get_unmanaged_process( unmanagedProcessIndex );

            if( m_existingElements.find( processParser.name().c_str() ) ==
                m_existingElements.end() )
            {
                m_existingElements.insert(
                    PlatformMngt::ExistingElementMap::value_type(
                        processParser.name().c_str(),
                        1 ) );
            }
        }

        // Check all the graph elements are defined
        // in the definition of the application
        PlatformMngt::GraphElementMap::iterator graphElementIt;
        
        for( graphElementIt = m_graphElementMap.begin();
             graphElementIt != m_graphElementMap.end();
             graphElementIt++ )
        {
            PlatformMngt::ExistingElementMap::iterator foundElement
                = m_existingElements.find( graphElementIt->second->name() );
            if( foundElement == m_existingElements.end() )
                throw CdmwPlatformMngt::ReferenceNotFound(
                    graphElementIt->second->name() );
            if( !ignoreStep &&
                !(graphElementIt->second->step() < foundElement->second) )
                throw CdmwPlatformMngt::StepOutOfBound(
                    graphElementIt->second->name(),
                    graphElementIt->second->step() );
        }

        // Do more verifications on the graph
        PlatformMngt::GraphChecker processGraphChecker(
            m_roots.in(), &m_existingElements, ignoreStep );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

