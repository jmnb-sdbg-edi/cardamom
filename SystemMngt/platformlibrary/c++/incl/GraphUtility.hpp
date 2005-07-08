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


#ifndef INCL_PLATFORMMNGT_GRAPH_UTILITY_HPP
#define INCL_PLATFORMMNGT_GRAPH_UTILITY_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"
#include <set>
#include <string>

namespace Cdmw
{
namespace PlatformMngt
{

/**
 *Purpose:
 *<p> Set of graph elements' name.
 *
 *Features:
 <p>
 */
typedef std::set< std::string > ElementNames;

/**
 *Purpose:
 *<p> Utility for task sequencing based on dependency graph.
 *
 *Features:
 *<p>
 */        
class GraphUtility
{
public:
    /**
     *Purpose:
     *<p> Gets the task definitions and the root tasks corresponding to
     * the graph elements. if ignoreStep is TRUE, the step member of each
     * graph element is not used for identifying the task. Otherwise, the
     * identifier of a task is the concatenation of the element name and
     * the step number.
     *
     *@param graphelements The dependency graph.
     *@param ignoreStep Indicates wether the step number has to be used or
     * not for the task identification.
     *@param taskDefMap On return, this parameter contains the map of all
     * the graph element's identification associated with the corresponding
     * synchronisable tasks.
     *@param rootTasks On return, this parameter contains the task list of
     * the graph root elements.
     */
    static
    void getTaskDefinitions(
        const CdmwPlatformMngt::GraphElements& graphElements,
        bool ignoreStep,
        SynchronisableTaskDefMap& taskDefMap,
        TaskIdList& rootTasks );

    /**
     *Purpose:
     *<p> Gets the set of the element names. The step number of each graph
     * element is not used.
     *
     *@param graphelements The dependency graph.
     *
     *@return The set of element names.
     */
    static
    ElementNames getElementNames(
        const CdmwPlatformMngt::GraphElements& graphElements );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_GRAPH_UTILITY_HPP

