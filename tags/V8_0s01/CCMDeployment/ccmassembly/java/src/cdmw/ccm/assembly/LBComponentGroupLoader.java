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


package cdmw.ccm.assembly;


import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import org.omg.Components.CreateFailure;


/**
 * This class loads the &lt;componentgroup&gt; XML elements.
 * A LBComponentGroup will be created and added to the assembly
 * for each of the &lt;componentgroup&gt; XML element parsed.
 */
class LBComponentGroupLoader extends ComponentGroupLoader {


    /**
     * Loads a &lt;lbcomponentgroup&gt; XML element and creates a
     * LBComponentGroup and adds the group to the assembly.
     *
     * @param componentGroupElt the &lt;lbcomponentgroup&gt; XML element.
     * @param assembly add the LBComponentGroup to this assembly.
     * @param assemblyPack the AssemblyPackage corresponding to
     *                     the specified assembly.
     *
     * @throws CreateFailure if the loading fails.
     */
    void load(Element componentGroupElt,
              AssemblyImpl assembly,
              _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // Get the id of the component group.
        String id = loadId(componentGroupElt);

        // Get the destination of the component group.
        String destination = loadDestination(componentGroupElt);

        // Create a LBComponentGroup,
        // it will be automatically added to the assembly.
        LBComponentGroup lbgroup =
            new LBComponentGroup(id, assembly, destination);

        loadGroupMembers(componentGroupElt, lbgroup);

        loadGroupProperties(lbgroup,
                            assembly.getConfiguratorManager(),
                            assemblyPack);

        loadNamingRegistrations(lbgroup, componentGroupElt);
    }


}
