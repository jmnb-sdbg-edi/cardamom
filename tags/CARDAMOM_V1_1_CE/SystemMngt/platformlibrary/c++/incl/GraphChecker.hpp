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


#ifndef INCL_PLATFORMMNGT_GRAPH_CHECKER_HPP
#define INCL_PLATFORMMNGT_GRAPH_CHECKER_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include <map>
#include <vector>

namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

/**
 * The map of graph elements and their number of steps.
 */
typedef std::map< std::string, unsigned short > ExistingElementMap;

/**
 * The map of graph element.
 */
typedef std::map< std::string, CdmwPlatformMngt::GraphElement_var >
    GraphElementMap;

/**
 *Purpose:
 *<p>
 */
class BaseGraphChecker
{

public:
    typedef std::vector< CdmwPlatformMngt::GraphElement_var > GraphElementPath;

protected:
    /**
     * The map of element's name and number of steps (used for a graph
     * of processes).
     */
    ExistingElementMap* m_existingElements;

    /**
     * The sequence of root elements of the graph.
     */
    CdmwPlatformMngt::GraphElements_var m_roots;

private:
    /**
     * Indicates if the step number has to be ignored.
     */
    bool m_ignoreStep;

    /**
     * Gets a reference to a CheckGraphElement.
     * This method is called by the method convertSubGraph().
     */
    CdmwPlatformMngt::GraphElement* getCheckGraphElement(
        std::string name,
        unsigned short step,
        GraphElementMap& map )
            throw( OutOfMemoryException );

    /**
     * Recursively converts the graph into a graph of CheckGraphElement.
     * This method is called by the method convertGraph().
     */
    CdmwPlatformMngt::GraphElement* convertSubGraph(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementMap& graphElementMap )
            throw( OutOfMemoryException );

    /**
     * Converts the graph into a graph of CheckGraphElement(s).
     * This method is called in private before checking there is
     * no step jumps.
     */
    CdmwPlatformMngt::GraphElements* convertGraph(
        const CdmwPlatformMngt::GraphElements& graphRootElements )
            throw( OutOfMemoryException );

    /**
     * Recursively checks the validity of a specified sub graph. This
     * method applies the abstract check_graph_element() method to
     * the specified sub graph and to all its sub elements.
     *
     * Override the check_graph_element() method to provide a concrete
     * verification of each element of the graph.
     *
     *@param graphElement the graph element to be verified.
     *@param graphElementPath the path (in the graph) of the element to be
     * verified.
     *@param graphElementRootPath the path (in the graph) of the root elements excluding
     *       the root element of the current branch.
     *
     *@exception OutOfMemoryException if memory lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepOutOfBound if the graph element's step number not in
     * the defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    void check_sub_graph(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementPath& graphElementPath,
        GraphElementPath& graphElementRootPath )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );

protected:
    /**
     * Creates a graph checker with the designated graph root
     * elements. No verification of the graph is applied.
     *
     *@param graphElements the whole graph to be verified.
     *@param existingElements The map of element's name and number of steps
     * (used for a graph of processes).
     *@param ignoreStep indicates if the step of the graph element has to be
     * ignored.
     *
     *@exception OutOfMemoryException if memory lacking.
     */
    BaseGraphChecker(
        const CdmwPlatformMngt::GraphElements& graphRootElements,
        ExistingElementMap* existingElements,
        bool ignoreStep )
            throw( OutOfMemoryException,
                   CORBA::SystemException );

    /**
     * Releases this graph checker.
     */
    virtual
    ~BaseGraphChecker();

    /**
     * Recursively checks the specified graph has no step jump.
     * This method looks for no step jump through out the 
     * specified sub graph and to all its sub elements.
     *
     *@param graphElement the graph element to be verified.
     *
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    void check_sub_graph_for_no_step_jump(
        CdmwPlatformMngt::GraphElement* graphElement )
            throw( CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );

    /**
     * Abstract verification of a specified graph element's features.
     * This method is called when checking the validity of the whole
     * graph.
     *
     * Override this method to provide a concrete verification of the
     * specified graph element.
     *
     *@param graphElement the graph element to be verified.
     *@param graphElementPath the path (in the graph) of the element to be
     *       verified.
     *@param graphElementRootPath the path (in the graph) of the root elements excluding
     *       the root element of the current branch.
     *
     *@exception OutOfMemoryException if memory lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepOutOfBound if the graph element's step number out of the
     * defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    virtual
    void check_graph_element(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementPath& graphElementPath,
        GraphElementPath& graphElementRootPath )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException ) = 0;

    /**
     * Checks a graph element's process is existent.
     *
     *@param graphElement the graph element to be verified.
     */
    bool check_element_existence(
        CdmwPlatformMngt::GraphElement* graphElement )
            throw( CORBA::SystemException );

    /**
     * Checks a graph element's step is in the range of steps for
     * a graph element's process.
     *
     *@param graphElement the graph element to be verified.
     */
    bool check_step_boundaries(
        CdmwPlatformMngt::GraphElement* graphElement )
            throw( CORBA::SystemException );

    /**
     * Checks the circular reference of a specified element in a specified
     * path. Use this method to verify a path not cyclic.
     *
     *@param graphElement the graph element to be verified.
     *@param graphElementPath the path (in the graph) of the element to be
     *       verified.
     *@param graphElementRootPath the path (in the graph) of the root elements excluding
     *       the root element of the current branch.
     */
    bool check_circular_reference(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementPath& graphElementPath,
        GraphElementPath& graphElementRootPath )
            throw( CORBA::SystemException );

    /**
     * Checks a specified element's step is valid in a specified path.
     * Use this method to verify the path'steps are consecutive.
     *
     *@param graphElement the graph element to be verified.
     *@param graphElementPath the path (in the graph) of the element to be
     * verified.
     */
    void check_no_step_jump(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementPath& graphElementPath )
            throw( CORBA::SystemException );

public:
    /**
     * Checks the whole graph from its root elements.
     *
     *@exception OutOfMemoryException if memory lack.
     *@exception ReferenceNotFound if a graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepOutOfBound if a graph element's step number out of the
     * defined range.
     *@exception StepJump if a graph element's step number is not higher
     * than its predecessor's step number.
     */
    void check()
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p>
 */
class GraphChecker : virtual public BaseGraphChecker
{
public:
    /**
     * Checks the specified graph of applications. It is designated by its
     * root graph elements.
     * A verification of the graph is applied.
     *
     *@param graphElements the whole graph to be verified.
     *@param existingElements The map of element's name and number of steps
     * (used for a graph of processes).
     *@param ignoreStep indicates if the step of the graph element has to be
     * ignored.
     *
     *@exception OutOfMemoryException if memory lacking.
     *@exception ReferenceNotFound if a graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepOutOfBound if a graph element's step number out of the
     * defined range.
     *@exception StepJump if a graph element's step number is not higher
     * than its predecessor's step number.
     */
    GraphChecker(
        const CdmwPlatformMngt::GraphElements& graphElements,
        ExistingElementMap* existingElements,
        bool ignoreStep )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );

protected:
    /**
     * Checks the graph of applications is not cyclic. The existence of all
     * the applications of the graph are verified when the graph checker is
     * initialised with the map of applications.
     * This method is called when checking the validity of the whole graph.
     *
     *@param graphElement the graph element to be verified.
     *@param graphElementPath the path (in the graph) of the element to be
     *       verified.
     *@param graphElementRootPath the path (in the graph) of the root elements excluding
     *       the root element of the current branch.
     *
     *@exception OutOfMemoryException if memory lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepOutOfBound if the graph element's step number out of the
     * defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    void check_graph_element(
        CdmwPlatformMngt::GraphElement* graphElement,
        GraphElementPath& graphElementPath,
        GraphElementPath& graphElementRootPath )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_GRAPH_CHECKER_HPP

