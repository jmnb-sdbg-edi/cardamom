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


import cdmw.xmlutils.XMLHelper;

import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import com.thalesgroup.CdmwXML.Element;

import org.omg.Components.CreateFailure;


/**
 * This class loads the &lt;processcollocation&gt; XML elements having
 * &lt;loadbalanced&gt; as child node and creates theirs corresponding
 * LBComponentServer objects.
 */
class LBProcessCollocationLoader extends DefaultProcessCollocationLoader {


    /**
     * Constructor.
     */
    public LBProcessCollocationLoader(){
    }


    /**
     * This method loads a &lt;processcollocation&gt; XML element, creates its
     * corresponding LBComponentServer and adds it to its parent HostCollocation
     * as well as to the assembly.
     *
     * @param processElt   the &lt;processcollocation&gt; XML element.
     * @param assemblyPack the AssemblyPackage of the parent Assembly.
     * @param parentHome   the parent HostCollocation of the ComponentServer.
     *
     * @throws CreateFailure if the loading fails.
     */
    void load(Element processElt,
              _AssemblyPackage assemblyPack,
              HostCollocation parentHost)
        throws CreateFailure
    {
        // from the <processcollocation> XML element:

        // get the id attribute value
        String id = getProcesscollocationId(processElt);

        // get the destination node text value
        String destination = getProcesscollocationDestination(processElt, id);

        // create a new LBComponentServer
        // this ComponentServer will automatically be added to its parentHost
        // as well as to its parent assembly
        LBComponentServer server =
            new LBComponentServer(id, parentHost, destination);

        // load the <homeplacement> child nodes
        loadHomeplacements(processElt, assemblyPack, server);
    }


}
