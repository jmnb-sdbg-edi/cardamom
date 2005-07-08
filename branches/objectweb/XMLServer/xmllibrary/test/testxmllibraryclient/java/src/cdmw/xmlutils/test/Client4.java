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


package cdmw.xmlutils.test;

import cdmw.xmlutils.XMLHelper;

import cdmw.xmlutils.DocumentDescriptionImpl;
import cdmw.xmlutils.ElementDescriptionImpl;
import cdmw.xmlutils.AttrDescriptionImpl;
import cdmw.xmlutils.TextDescriptionImpl;
import cdmw.xmlutils.CDATASectionDescriptionImpl;

import cdmw.xmlutils.DocumentDescriptionFactoryImpl;
import cdmw.xmlutils.ElementDescriptionFactoryImpl;
import cdmw.xmlutils.AttrDescriptionFactoryImpl;
import cdmw.xmlutils.TextDescriptionFactoryImpl;
import cdmw.xmlutils.CDATASectionDescriptionFactoryImpl;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.DocumentHelper;
import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.Attr;
import com.thalesgroup.CdmwXML.AttrHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.Text;
import com.thalesgroup.CdmwXML.TextHelper;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.CDATASection;
import com.thalesgroup.CdmwXML.CDATASectionHelper;
import com.thalesgroup.CdmwXML.CDATASectionDescription;

public class Client4 extends Thread {

    public static final String IOR_FILE_NAME = "xml_server.ior";
    
    public XMLParser parser;

    public String filename;
    
    public long duration = 0;

    public Client4(XMLParser parser, String filename) {
        this.parser = parser;
        this.filename = filename;
    }

    public long getDuration() {
        return this.duration;
    }

    public void run() {
    
        try {

            System.out.println("Parsing a file (" + filename + ")...");
            long start = (new java.util.Date()).getTime();
            DocumentDescription docDesc = parser.parse(filename);
            long duration = (new java.util.Date()).getTime() - start;
            System.out.println("Parsing time(in ms): " + duration);
            this.duration = duration;
            Document doc = DocumentHelper.narrow(docDesc.reference());
            System.out.println("Closing file...");
            doc.close();
        
        } catch(com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException pe) {
            pe.printStackTrace();
        } catch(org.omg.CORBA.SystemException se) {
            se.printStackTrace();
        }
    
    }

}

