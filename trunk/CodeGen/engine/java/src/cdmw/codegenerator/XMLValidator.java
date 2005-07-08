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


package cdmw.codegenerator;


import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;


public class XMLValidator extends DefaultHandler {
    // Error code
    protected final int RET_FATAL = 2;

    // Feature ids
    protected static final String DTD_VALIDATION_FEATURE_ID =
        "http://xml.org/sax/features/validation";

    protected static final String SCHEMA_VALIDATION_FEATURE_ID =
        "http://apache.org/xml/features/validation/schema";

    // Default parser name
    protected static final String DEFAULT_PARSER_NAME =
        "org.apache.xerces.parsers.SAXParser";

    // Default constructor
    public XMLValidator() {
    }

    // ContentHandler methods
    public void startDocument()
        throws SAXException {
    }

    public void startElement(String uri,
                             String local,
                             String raw,
                             Attributes attrs)
        throws SAXException {
    }

    public void characters(char ch[], int start, int length)
        throws SAXException {
    }

    public void ignorableWhitespace(char ch[], int start, int length)
        throws SAXException {
    }

    public void processingInstruction(String target, String data)
        throws SAXException {
    }

    // ErrorHandler methods
    public void warning(SAXParseException ex)
        throws SAXException {
    }

    public void error(SAXParseException ex)
        throws SAXException {
        System.err.println("\nFAILURE : " + ex);
        System.exit(RET_FATAL);
    }
    public void fatalError(SAXParseException ex)
        throws SAXException {
        System.err.println("\nFAILURE : " + ex);
        System.exit(RET_FATAL);
    }

    /**
     * Main.
     */
    public static void main(String[] args) {
        XMLValidator validator = new XMLValidator();

        for (int i = 0; i < args.length; i++) {
            XMLReader parser = null;

            try {
                parser = XMLReaderFactory.createXMLReader(DEFAULT_PARSER_NAME);
                parser.setFeature(DTD_VALIDATION_FEATURE_ID, true);
                parser.setFeature(SCHEMA_VALIDATION_FEATURE_ID, true);
            } catch (SAXException e) {
                e.printStackTrace();
            }

            // Parse file
            parser.setContentHandler(validator);
            parser.setErrorHandler(validator);
            try {
                parser.parse(args[i]);
            } catch (Exception e) {
                Exception se = e;
                if (e instanceof SAXException) {
                    se = ((SAXException) e).getException();
                }
                if (se != null) {
                    se.printStackTrace(System.err);
                } else {
                    e.printStackTrace(System.err);
                }
            }
        }
    }
}

