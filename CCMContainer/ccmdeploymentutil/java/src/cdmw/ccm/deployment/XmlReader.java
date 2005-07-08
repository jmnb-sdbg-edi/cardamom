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


package cdmw.ccm.deployment;


import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.DocumentHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.XPathFormatException;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;

import cdmw.xmlutils.XMLHelper;



public class XmlReader {

    protected DocumentDescription docDescr;
    protected Document doc;
    
    
    public XmlReader(XMLParser parser, String xmlFilename) 
        throws ParsingException
    {
        docDescr = parser.parse(xmlFilename);
        doc = DocumentHelper.narrow(docDescr.reference());
    }
    
    
    public NodeDescription[] selectNodes(String xpath) 
        throws XPathFormatException
    {
        return doc.select_nodes_from_root(XMLHelper.toShortArray(xpath));
    }
    
    
    public void close() {
        if (doc != null) {
            doc.close();
            doc = null;
            docDescr = null;
        }
    }
    
    
    protected void finalize() {
        close();
    }
    
    
}



