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


#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{

std::string getTaskId(
    CdmwPlatformMngt::GraphElement* graphElement,
    bool ignoreStep )
{
    std::ostringstream taskId;

    // Determine the task id of the designated graph element
    if( !ignoreStep )
    {
        taskId << graphElement->name() << "\\" << graphElement->step();
    }
    else
    {
        taskId << graphElement->name();
    }

    return std::string( taskId.str() );
}

void getSubTaskDefMap(
    CdmwPlatformMngt::GraphElement* graphElement,
    bool ignoreStep,
    SynchronisableTaskDefMap& syncTaskDefMap )
{
    // Determine the task id of the current graph element
    std::string taskId = getTaskId( graphElement, ignoreStep );

    // Find the occurence of the task in the syncTaskDefMap map
    SynchronisableTaskDefMap::iterator iterator = syncTaskDefMap.find( taskId );
    if( iterator != syncTaskDefMap.end() )
    {
        // If task found
        return;
    }
    else
    {
        // If task not found,
        // create a syncTaskDef completed with the successor tasks list
        SynchronisableTaskDef syncTaskDef;
        for( unsigned int eltIndex=0; eltIndex < graphElement->successors().length(); eltIndex++ )
        {
            // Add the current successor task to the current task's successors list
            std::string successorTaskId = getTaskId(
                graphElement->successors()[ eltIndex ],
                ignoreStep );
            syncTaskDef.m_successors_id.push_back( successorTaskId );
        }
    
        // Add a new entry to the map with the ossociation of taskId and syncTaskDef
        syncTaskDefMap.insert( SynchronisableTaskDefMap::value_type( taskId, syncTaskDef ) );
    }

    // Go through the current element's successors
    for( unsigned int eltIndex=0; eltIndex < graphElement->successors().length(); eltIndex++ )
        getSubTaskDefMap( graphElement->successors()[ eltIndex ], ignoreStep, syncTaskDefMap );
}

void getSubEltNames(
    CdmwPlatformMngt::GraphElement* graphElement,
    ElementNames& elementNames )
{
    // Get the current graph element's name
    std::string newName = graphElement->name();

    // Look in the set for no occurence of the new name than add it to the set
    if( elementNames.find( newName ) == elementNames.end() )
        elementNames.insert( newName );

    // Get all the current graph element's successor names
    for( unsigned int eltIndex=0; eltIndex < graphElement->successors().length(); eltIndex++ )
        getSubEltNames( graphElement->successors()[ eltIndex ], elementNames );
}

// ----------------------------------------------------------------------
// GraphUtility class.
// ----------------------------------------------------------------------
void GraphUtility::getTaskDefinitions(
    const CdmwPlatformMngt::GraphElements& graphElements,
    bool ignoreStep,
    SynchronisableTaskDefMap& taskDefMap,
    TaskIdList& rootTasks )
{
    // Build the root task id list and taskDefMap including the successors list
    for( unsigned int eltIndex=0; eltIndex<graphElements.length(); eltIndex++ )
    {
        // Build the whole map TaskDefMap from the current root element
        getSubTaskDefMap( graphElements[ eltIndex ], ignoreStep, taskDefMap );

        // Build the task id list of the root elements
        std::string taskId = getTaskId( graphElements[ eltIndex ], ignoreStep );
        rootTasks.push_back( taskId );
    }

    // Complete the map with the amount of predecessors for each entry
    SynchronisableTaskDefMap::iterator taskIt;
    for( taskIt=taskDefMap.begin(); taskIt!=taskDefMap.end(); taskIt++ )
    {
        // Get the current map entry's task id
        std::string taskId = taskIt->first;

        // Go through all the map entry 
        SynchronisableTaskDefMap::iterator taskIt2;
        for( taskIt2=taskDefMap.begin(); taskIt2!=taskDefMap.end(); taskIt2++ )
        {
            // Look in successors list of each map entry for the task taskId
            TaskIdList::iterator successorIt;
            for( successorIt =  taskIt2->second.m_successors_id.begin();
                 successorIt != taskIt2->second.m_successors_id.end();
                 successorIt++ )
            {
                // If found, increment the m_nbPredecessors of 1 unit
                if( *successorIt == taskId )
                    taskIt->second.m_nbPredecessors++;
            }
        }
    }
}

ElementNames GraphUtility::getElementNames(
    const CdmwPlatformMngt::GraphElements& graphElements )
{
    ElementNames elementNames;

    // Go through the whole graph from each root element
    for( unsigned int eltIndex=0; eltIndex < graphElements.length(); eltIndex++ )
        getSubEltNames( graphElements[ eltIndex ], elementNames );

    return elementNames;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

