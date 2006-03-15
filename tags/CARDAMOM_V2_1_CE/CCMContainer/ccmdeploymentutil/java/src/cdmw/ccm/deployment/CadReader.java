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


package cdmw.ccm.deployment;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.XPathFormatException;

import cdmw.xmlutils.XMLHelper;

import org.omg.Components.Deployment.InstallationFailure;


public class CadReader extends XmlReader {

    private static final String CAD_DOCTYPE = "componentassembly";

    private static final String XPATH_ID_ATTR = "/componentassembly/@id";


    public CadReader(XMLParser parser, String cadFilename) 
        throws ParsingException
    {
        super(parser, cadFilename);
        
        if (! XMLHelper.toString(docDescr.doctype()).equals(CAD_DOCTYPE) ) {
            throw new ParsingException(
                cadFilename + " is not a .cad file.");
        }
    }


    public String getUUID() {
        String uuid = null;
        try {
            NodeDescription[] nodes = selectNodes(XPATH_ID_ATTR);

            // an id attribute must have been found
            // (cad is validated by dtd, and id is required)
            AttrDescription idAttr = (AttrDescription) nodes[0];
        
            uuid = XMLHelper.toString(idAttr.value());
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        return uuid;
    }
    


}

