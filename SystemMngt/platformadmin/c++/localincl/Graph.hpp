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


#ifndef INCL_PLATFORMADMIN_GRAPH_HPP
#define INCL_PLATFORMADMIN_GRAPH_HPP

#include <string>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class GraphParser;
class GraphNodeParser;
class SystemParser;
class ApplicationParser;

/**
 *Purpose:
 *<p>
 */
class Graph
{
protected:
    /**
     * The root elements of the graph.
     */
    CdmwPlatformMngt::GraphElements_var m_roots;

    /**
     * The map of references to all the elements of the graph.
     */
    PlatformMngt::GraphElementMap m_graphElementMap;

    /**
     * The map of element's name and number of steps.
     */
    PlatformMngt::ExistingElementMap m_existingElements;

private:
    /**
     *Purpose:
     *<p> Gets a reference to a designated graph element. This method
     * avoids to have multiple references to a graph element.
     *
     *@param name the name of the graph element.
     *@param host the host of the graph element.
     *@param step the step of the graph element.
     *
     *@return the reference to the graph element.
     *
     *@exception OutOfMemoryException if memory is lacking.
     */
    CdmwPlatformMngt::GraphElement* get_graph_element(
        std::string name,
        std::string host,
        unsigned short step );

protected:
    /**
     *Purpose:
     *<p> Creates the graph specified by a GraphParser. No verification
     * is applied.
     *
     *@param graphParser the XML parser of graph definition.
     *
     *@exception OutOfMemoryException if memory is lacking.
     *@exception ConfigException if an error occured while parsing the
     * the XML definition of the graph.
     */
    Graph(
        const GraphParser& graphParser );

    /**
     *Purpose:
     *<p> Releases this Graph.
     */
    virtual ~Graph();

public:
    /**
     *Purpose:
     *<p> Gets the root elements of the graph.
     *
     *@return the root elements of the graph.
     */
    CdmwPlatformMngt::GraphElements* get_roots() const;
};

/**
 *Purpose:
 *<p>
 */
class ApplicationGraph : virtual public Graph
{
public:
    /**
     *Purpose:
     *<p> Creates the application graph designated by a GraphParser.
     * A verification of the created graph is applied.
     *
     *@param graphParser the XML graph definition parser.
     *
     *@exception ConfigException if an error occured while parsing the
     *@exception OutOfMemoryException if memory is lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepRangeMap if the graph element's step number not in
     * the defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    ApplicationGraph(
        const GraphParser& graphParser );

    /**
     *Purpose:
     *<p> Creates the application graph designated by a SystemParser.
     * The SystemParser contains all the applications with their
     * name.
     *
     *@param systemParser the XML system definition parser.
     *@param graphParser the XML graph definition parser.
     *
     *@exception ConfigException if an error occured while parsing the
     *@exception OutOfMemoryException if memory is lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepRangeMap if the graph element's step number not in
     * the defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    ApplicationGraph(
        const SystemParser& systemParser,
        const GraphParser& graphParser );
};

/**
 *Purpose:
 *<p>
 */
class ProcessGraph : virtual public Graph
{
public:
    /**
     *Purpose:
     *<p> Creates the process graph designated by a GraphParser.
     *
     *@param graphParser the XML parser of graph definition.
     *@param ignoreStep indicates if the step of the graph element has to be
     * ignored.
     *
     *@exception ConfigException if an error occured while parsing the
     *@exception OutOfMemoryException if memory is lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepRangeMap if the graph element's step number not in
     * the defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    ProcessGraph(
        const GraphParser& graphParser,
        bool ignoreStep );

    /**
     *Purpose:
     *<p> Creates the process graph designated by an ApplicationParser.
     * The ApplicationParser contains all the processes with their
     * name and number of step.
     *
     *@param applicationParser the XML application definition parser.
     *@param graphParser the XML parser of graph definition.
     *@param ignoreStep indicates if the step of the graph element has to be
     * ignored.
     *
     *@exception ConfigException if an error occured while parsing the
     *@exception OutOfMemoryException if memory is lacking.
     *@exception ReferenceNotFound if the graph element's name not in the map.
     *@exception CircularReference if the graph is cyclic.
     *@exception StepRangeMap if the graph element's step number not in
     * the defined range.
     *@exception StepJump if the graph element's step number is not higher
     * than its predecessor's step number.
     */
    ProcessGraph(
        const ApplicationParser& applicationParser,
        const GraphParser& graphParser,
        bool ignoreStep );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_GRAP_HPP

