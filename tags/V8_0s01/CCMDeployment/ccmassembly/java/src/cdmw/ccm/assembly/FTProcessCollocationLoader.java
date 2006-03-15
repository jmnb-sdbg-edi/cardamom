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

import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


/**
 * This class loads processcollocation XML elements which 
 * have a faulttolerant child XML element, and create a new 
 * corresponding FTComponentServer object.
 */
class FTProcessCollocationLoader extends DefaultProcessCollocationLoader 
{
    
    private static final String XPATH_FTPRIMARY_ATTRIBUTE =
        "faulttolerant/@ftprimary";

    
    /**
     * Constructor.
     */
    public FTProcessCollocationLoader() {
    }
   
    
    /**
     * Loads a processcollocation XML element, create a corresponding FTComponentServer
     * and add it to its parent HostCollocation and to the assembly.
     *
     * @param processElt the processcollocation XML element
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param parentHome the parent HostCollocation of the ComponentServer.
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element processElt,
              _AssemblyPackage assemblyPack,
              HostCollocation parentHost)
        throws CreateFailure
    {
        // get id of processcollocation
        String id = getProcesscollocationId(processElt);
        // get destination of processcollocation
        String destination = getProcesscollocationDestination(processElt, id);

        // check if processcollocation is declared to be primary
        boolean primary = isPrimary(processElt, id);
        
        // create FTComponentServer (automatically added to its  
        // parentHost and to its parent assembly).
        FTComponentServer server = new FTComponentServer(id, parentHost, destination, primary);

        // load children homeplacements
        loadHomeplacements(processElt, assemblyPack, server);
    }    

    /**
     * Loads the ftprimary attribute of the faulttolerant
     * XML element and returns its value.
     *
     * @param processElt the parent processcollocation XML element.
     * @param id the identifier of the processcollocation.
     * @return the value of the ftprimary attribute.
     *
     * @exception CreateFailure if loading fails.
     */
    private boolean isPrimary(Element processElt,
                              String id)
            throws CreateFailure
    {
        try {
            // get ftprimary attribute of <faulttolerant> element
            NodeDescription[] ftprimaryNodes = processElt.select_nodes(
                XMLHelper.toShortArray(XPATH_FTPRIMARY_ATTRIBUTE));
            
            // if ftprimary attribute is not specified
            if (ftprimaryNodes.length == 0) {
                // default value is false
                return false;
            }
                
            // get value of ftprimary attribute
            AttrDescription ftprimaryDescr = (AttrDescription) ftprimaryNodes[0];
            String ftprimary = XMLHelper.toString(ftprimaryDescr.value());
            
            if (ftprimary.equals("true")) {
                return true;
            } else if (ftprimary.equals("false")) {
                return false;
            } else {
                // shouldn't happen, only true and false are valid
                cdmw.common.Assert.check(false);
                // for compiler (unused)
                return false;
            }
                
        } catch (XPathFormatException e) {
            throw new CreateFailure(
                "No faulttolerant element with ftprimary attribute "+
                "in processcollocation " + id,
                ERROR_IN_CAD.value);
        }          
    }
    
}


