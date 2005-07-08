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


#ifndef INCL_PLATFORMADMIN_GRAPHPARSER_HPP
#define INCL_PLATFORMADMIN_GRAPHPARSER_HPP

#include "platformadmin/ConfigElementParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class GraphNodeParser : public virtual ConfigElementParser
{
private:
    class GraphSuccessorNodeParser : public virtual ConfigElementParser
    {
    public:
        GraphSuccessorNodeParser( const DOM_Node& graphSuccessorNodeParser );

        std::string name() const;

        unsigned short step() const;
    };

public:
    GraphNodeParser( const DOM_Node& graphNodeParser );

    std::string name() const;

    unsigned short step() const;

    unsigned int get_successor_node_count() const;

    std::string get_successor_node_name( unsigned int successorNodeIndex ) const;

    unsigned short get_successor_node_step( unsigned int successorNodeIndex ) const;
};

class GraphParser : public virtual ConfigElementParser
{
private:
    class GraphRootNodeParser : public virtual ConfigElementParser
    {
    public:
        GraphRootNodeParser( const DOM_Node& graphRootNodeParser );

        std::string name() const;

        unsigned short step() const;
    };

public:
    GraphParser( const DOM_Node& graphParser );

    unsigned int get_graph_node_count() const;

    GraphNodeParser get_graph_node( unsigned int graphNodeIndex ) const;

    unsigned int get_root_node_count() const;

    std::string get_root_node_name( unsigned int rootNodeIndex ) const;

    unsigned short get_root_node_step( unsigned int rootNodeIndex ) const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_GRAPHPARSER_HPP

