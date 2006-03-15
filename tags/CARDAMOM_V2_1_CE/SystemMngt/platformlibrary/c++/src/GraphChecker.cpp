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


#include <iostream>
#include <sstream>

#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "platformlibrary/CheckGraphElement_impl.hpp"

#include "SystemMngt/platformlibrary/GraphChecker.hpp"

namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

// ----------------------------------------------------------------------
// Graph class.
// ----------------------------------------------------------------------
CdmwPlatformMngt::GraphElement* BaseGraphChecker::getCheckGraphElement(
    std::string name,
    unsigned short step,
    GraphElementMap& map )
throw( OutOfMemoryException )
{
    try
    {
        CdmwPlatformMngt::GraphElement_var graphElement;

        // Search the specified graph element
        std::ostringstream key;

        key << name << "-" << step << std::ends;
        GraphElementMap::iterator iterator
            = map.find( key.str() );

        if( iterator != map.end() )
        {
            // Found element, get its reference
            graphElement = iterator->second;
        }
        else
        {
            // Not found element, create a new reference
            #if CDMW_ORB_VDR == tao
            Cdmw::PlatformMngt::CheckGraphElementFactory checkGraphElementFactory;

            graphElement
                = checkGraphElementFactory.create( name.c_str(), step );
            #else
            CORBA::ValueFactoryBase_var checkGraphElementFactory
                = new Cdmw::PlatformMngt::CheckGraphElementFactory();

            graphElement
                = dynamic_cast< Cdmw::PlatformMngt::CheckGraphElementFactory* >
                    ( checkGraphElementFactory.in() )->create( name.c_str(), step );
            #endif

            // Add it to the map of graph element references
            map.insert(
                GraphElementMap::value_type(
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

CdmwPlatformMngt::GraphElement* BaseGraphChecker::convertSubGraph(
    CdmwPlatformMngt::GraphElement* graphElement,
    GraphElementMap& graphElementMap )
throw( OutOfMemoryException )
{
    CdmwPlatformMngt::GraphElement_var newGraphElement;

    // Search the current graph element in the graphElementMap
    std::ostringstream key;

    key << graphElement->name() << "-" << graphElement->step() << std::ends;
    GraphElementMap::iterator iterator
        = graphElementMap.find( key.str() );

    // Get a CheckGraphElement reference of the current GraphElement
    newGraphElement
        = getCheckGraphElement(
            graphElement->name(),
            graphElement->step(),
            graphElementMap );

    // Found element, stop recursion
    if( iterator != graphElementMap.end() )
        return newGraphElement._retn();

    // Set the number of successors
    unsigned int successorCount = graphElement->successors().length();

    newGraphElement->successors().length( successorCount );

    // Go through the successors
    for( unsigned int grapElementIndex = 0;
         grapElementIndex < successorCount;
         grapElementIndex++ )
    {
        newGraphElement->successors()[ grapElementIndex ]
            = convertSubGraph(
                graphElement->successors()[ grapElementIndex ],
                graphElementMap );
    }

    return newGraphElement._retn();
}

CdmwPlatformMngt::GraphElements* BaseGraphChecker::convertGraph(
    const CdmwPlatformMngt::GraphElements& graphRootElements )
throw( OutOfMemoryException )
{
    CdmwPlatformMngt::GraphElements_var newGraphRootElements
        = new CdmwPlatformMngt::GraphElements();

    newGraphRootElements->length( graphRootElements.length() );

    // Convert the sub elements of the roots
    GraphElementMap graphElementMap;

    for( unsigned int rootIndex = 0;
         rootIndex < graphRootElements.length();
         rootIndex++ )
    {
        newGraphRootElements[ rootIndex ]
            = convertSubGraph(
                graphRootElements[ rootIndex ],
                graphElementMap );
    }

    return newGraphRootElements._retn();
}

void BaseGraphChecker::check_sub_graph(
    CdmwPlatformMngt::GraphElement* graphElement,
    GraphElementPath& graphElementPath,
    GraphElementPath& graphElementRootPath )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    // Check the current graph element
    check_graph_element( graphElement, graphElementPath, graphElementRootPath );

    // Update the current graph element's path
    
    /**** avoid sequence copy in _copy_value() so use factory to create value type
    #if CDMW_ORB_VDR == tao
    GraphElement_impl* graphElement_i
        = dynamic_cast< GraphElement_impl* >( graphElement );

    graphElementPath.push_back(
        CdmwPlatformMngt::GraphElement::_downcast(
            graphElement_i->_copy_value() ) );
    #else
    graphElementPath.push_back(
        CdmwPlatformMngt::GraphElement::_downcast(
            graphElement->_copy_value() ) );
    #endif
    *****/
    
    CdmwPlatformMngt::GraphElement* graphElement_copy;
    
    #if CDMW_ORB_VDR == tao
    Cdmw::PlatformMngt::CheckGraphElementFactory checkGraphElementFactory;

    graphElement_copy = checkGraphElementFactory.create (graphElement->name(), graphElement->step());
    #else
    CORBA::ValueFactoryBase_var checkGraphElementFactory =
                     new Cdmw::PlatformMngt::CheckGraphElementFactory();

    graphElement_copy = dynamic_cast <Cdmw::PlatformMngt::CheckGraphElementFactory*>
                    (checkGraphElementFactory.in())->create (graphElement->name(), graphElement->step());
    #endif
     
    graphElementPath.push_back (graphElement_copy);      
    

    // Check all the current graph element's successors
    for( unsigned int successorIndex = 0;
         successorIndex < graphElement->successors().length();
         successorIndex++ )
    {
        check_sub_graph(
            graphElement->successors()[successorIndex],
            graphElementPath, graphElementRootPath );
    }

    // Forget the update of the current graph element's path
    graphElementPath.pop_back();
}

BaseGraphChecker::BaseGraphChecker(
    const CdmwPlatformMngt::GraphElements& graphRootElements,
    ExistingElementMap* existingElements,
    bool ignoreStep )
throw( OutOfMemoryException,
       CORBA::SystemException )
{
    try
    {
        m_ignoreStep = ignoreStep;
        m_existingElements = existingElements;

        if( ignoreStep )
        {
            m_roots = new CdmwPlatformMngt::GraphElements( graphRootElements );
        }
        else
        {
            // Transform the graph into a grap of CheckGraphElement in order
            // to be used by the method check_no_step_jump().
            m_roots = convertGraph( graphRootElements );
        }
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

BaseGraphChecker::~BaseGraphChecker()
{
}

void BaseGraphChecker::check_sub_graph_for_no_step_jump(
    CdmwPlatformMngt::GraphElement* graphElement )
throw( CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    // Check the current element for no step jump
    CdmwPlatformMngt::CheckGraphElement* checkGraphElement
        = CdmwPlatformMngt::CheckGraphElement::_downcast( graphElement );

    if( !checkGraphElement->no_step_jump() )
        throw CdmwPlatformMngt::StepJump(
            checkGraphElement->name(),
            checkGraphElement->last_step(),
            checkGraphElement->step() );

    // Check all the current graph element's successors
    for( unsigned int successorIndex = 0;
         successorIndex < checkGraphElement->successors().length();
         successorIndex++ )
    {
        check_sub_graph_for_no_step_jump(
            checkGraphElement->successors()[successorIndex] );
    }
}

bool BaseGraphChecker::check_element_existence(
    CdmwPlatformMngt::GraphElement* graphElement )
throw( CORBA::SystemException )
{
    if( m_existingElements == NULL )
        return true;

    ExistingElementMap::iterator iterator
        = m_existingElements->find( graphElement->name() );
        
    if( iterator != m_existingElements->end() )
        return true;

    return false;
}

bool BaseGraphChecker::check_step_boundaries(
    CdmwPlatformMngt::GraphElement* graphElement )
throw( CORBA::SystemException )
{
    if( m_ignoreStep || (m_existingElements == NULL) )
        return true;

    ExistingElementMap::iterator iterator
        = m_existingElements->find( graphElement->name() );
        
    if( ( iterator != m_existingElements->end() ) &&
        ( graphElement->step() < iterator->second ) )
        return true;

    return false;
}

bool BaseGraphChecker::check_circular_reference(
    CdmwPlatformMngt::GraphElement* graphElement,
    GraphElementPath& graphElementPath,
    GraphElementPath& graphElementRootPath )
throw( CORBA::SystemException )
{
    unsigned int pathIndex;
    
    for (pathIndex=0;
         pathIndex < graphElementPath.size();
         pathIndex++)
    {
        CdmwPlatformMngt::GraphElement* pathElement
            = graphElementPath[pathIndex];
            
        if (m_ignoreStep)
        {
            if (!strcmp (pathElement->name(), graphElement->name()))
                return true;
        }
        else
        {
            if ((!strcmp( pathElement->name(), graphElement->name())) &&
                (pathElement->step() == graphElement->step()))
                return true;
        }
    }
    
    for (pathIndex=0;
         pathIndex < graphElementRootPath.size();
         pathIndex++)
    {
        CdmwPlatformMngt::GraphElement* pathElement
            = graphElementRootPath[pathIndex];
            
        if (m_ignoreStep)
        {
            if (!strcmp (pathElement->name(), graphElement->name()))
                return true;
        }
        else
        {
            if ((!strcmp( pathElement->name(), graphElement->name())) &&
                (pathElement->step() == graphElement->step()))
                return true;
        }
    }

    return false;
}

void BaseGraphChecker::check_no_step_jump(
    CdmwPlatformMngt::GraphElement* graphElement,
    GraphElementPath& graphElementPath )
throw( CORBA::SystemException )
{
    if( m_ignoreStep )
        return;

    CdmwPlatformMngt::CheckGraphElement* checkGraphElement
        = CdmwPlatformMngt::CheckGraphElement::_downcast( graphElement );

    for( unsigned int pathIndex = graphElementPath.size();
         pathIndex > 0;
         pathIndex-- )
    {
        CdmwPlatformMngt::GraphElement* pathElement
            = graphElementPath[ pathIndex-1 ];

        if( !strcmp( pathElement->name(), checkGraphElement->name() ) )
        {
            if( pathElement->step() != (checkGraphElement->step()-1) )
                checkGraphElement->last_step( pathElement->step() );
            else
                checkGraphElement->no_step_jump( true );

            return;
        }
    }

    // if first occurence of the graph element
    if( checkGraphElement->step() == 0 )
        checkGraphElement->no_step_jump( true );
}

void BaseGraphChecker::check()
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    GraphElementPath graphElementPath;
    GraphElementPath graphElementRootPath;

    for (unsigned int rootIndex = 0;
         rootIndex < m_roots->length();
         rootIndex++)
    {
        // store all the root graph element except the current one selected by rootIndex
        for (unsigned int rootIndex2 = 0;
             rootIndex2 < m_roots->length();
             rootIndex2++)
        {
            if (rootIndex2 != rootIndex)
            {
                CdmwPlatformMngt::GraphElement* graphElement_copy;
                
                CdmwPlatformMngt::GraphElement* graphElement = m_roots[rootIndex2];
    
                #if CDMW_ORB_VDR == tao
                Cdmw::PlatformMngt::CheckGraphElementFactory checkGraphElementFactory;

                graphElement_copy = 
                   checkGraphElementFactory.create (graphElement->name(), 
                                                    graphElement->step());
                #else
                CORBA::ValueFactoryBase_var checkGraphElementFactory =
                     new Cdmw::PlatformMngt::CheckGraphElementFactory();

                graphElement_copy = 
                   dynamic_cast <Cdmw::PlatformMngt::CheckGraphElementFactory*>
                    (checkGraphElementFactory.in())->create (graphElement->name(), 
                                                             graphElement->step());
                #endif
     
                graphElementRootPath.push_back (graphElement_copy); 
            }
            
        }
    
        check_sub_graph (m_roots[rootIndex], graphElementPath, graphElementRootPath);
        
        // empty the vector of root graph elements
        graphElementRootPath.clear();
    }

    // Check the whole graph for no step jump
    if (!m_ignoreStep)
    {
        for (unsigned int rootIndex = 0;
             rootIndex < m_roots->length();
             rootIndex++)
        {
            check_sub_graph_for_no_step_jump (m_roots [rootIndex]);
        }
    }
}

// ----------------------------------------------------------------------
// GraphChecker class.
// ----------------------------------------------------------------------
GraphChecker::GraphChecker(
    const CdmwPlatformMngt::GraphElements& graphElements,
    ExistingElementMap* existingElements,
    bool ignoreStep )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
: BaseGraphChecker( graphElements, existingElements, ignoreStep )
{
    check();
}

void GraphChecker::check_graph_element(
    CdmwPlatformMngt::GraphElement* graphElement,
    GraphElementPath& graphElementPath,
    GraphElementPath& graphElementRootPath )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    // Check the current graph element's existence
    if (!check_element_existence (graphElement))
        throw CdmwPlatformMngt::ReferenceNotFound(
            graphElement->name() );

    // Check the current graph element's step is in the defined range
    if (!check_step_boundaries (graphElement))
        throw CdmwPlatformMngt::StepOutOfBound (
            graphElement->name(),
            graphElement->step());

    // Check the path is not cyclic
    if (check_circular_reference (graphElement, graphElementPath, graphElementRootPath))
        throw CdmwPlatformMngt::CircularReference (
            graphElement->name(),
            graphElement->step());

    // Check there is no step jump
    check_no_step_jump (graphElement, graphElementPath);
}

} // End namespace PlatformMngt
} // End namespace Cdmw

