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


package cdmw.ccm.assembly;


import org.omg.Components.CreateFailure;

import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwDeployment._AssemblyPackage;


/**
 * This class loads componentgroup XML elements which
 * are child of a faulttolerance XML element.
 * For each of them, a FTComponentGroup object is created
 * and added to the assembly. 
 */
class FTComponentGroupLoader extends ComponentGroupLoader 
{
    
    /**
     * Load a componentgroup XML element, create a corresponding
     * FTComponentGroup and add it to the assembly.
     *
     * @param componentGroupElt the componentgroup XML element
     * @param assembly the assembly to which the FTComponentGroup
     *        has to be added.
     * @param assemblyPack the AssemblyPackage corresponding
     *        to the assembly
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element componentGroupElt,
              AssemblyImpl assembly,
              _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // get id of componentgroup
        String id = loadId(componentGroupElt);

        // get destination of componentgroup
        String destination = loadDestination(componentGroupElt);
        
        // create FTComponentGroup (automatically added to assembly)
        FTComponentGroup ftgroup = new FTComponentGroup(id, assembly, destination);
        
        loadGroupMembers(componentGroupElt,
                         ftgroup);
        
        loadGroupProperties(ftgroup,
                            assembly.getConfiguratorManager(),
                            assemblyPack);
                            
        loadNamingRegistrations(ftgroup,
                                componentGroupElt);
    }


}
