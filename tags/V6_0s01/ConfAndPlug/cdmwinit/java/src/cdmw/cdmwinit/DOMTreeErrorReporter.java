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


package cdmw.cdmwinit;

import org.xml.sax.SAXParseException;

public class DOMTreeErrorReporter implements org.xml.sax.ErrorHandler {

    // ----------------------------------------------------------------
    //  Implementation of the error handler interface
    // ----------------------------------------------------------------

    /**
     * Displays a warning message
     *
     * @param ex SAX Parser Exception
     */
    public void warning(SAXParseException ex) {
        System.err.println("[Warning] "+ getLocationString(ex)
                            + ": " + ex.getMessage());
    }

    /**
     * Displays an error message
     *
     * @param ex SAX Parser Exception
     */
    public void error(SAXParseException ex) {

        System.err.println("[Error] " + getLocationString(ex)
                            + ": " + ex.getMessage());
        // is there anything else to do ?

    }

    /**
     * Displays a fatal error message
     *
     * @param ex SAX Parser Exception
     */
    public void fatalError(SAXParseException ex)
        throws org.xml.sax.SAXException {

        System.err.println("[Fatal Error] " + getLocationString(ex)
                            + ": " + ex.getMessage());
        throw ex;

    }

    /**
     * Returns a string of the location.
     *
     * @param ex SAX Parser Exception
     *
     * @return The location.
     */
    private String getLocationString(SAXParseException ex) {
        StringBuffer str = new StringBuffer();

        String systemId = ex.getSystemId();
        if (systemId != null) {
            int index = systemId.lastIndexOf('/');
            if (index != -1)
                systemId = systemId.substring(index + 1);
            str.append(systemId);
        }
        str.append(':');
        str.append(ex.getLineNumber());
        str.append(':');
        str.append(ex.getColumnNumber());

        return str.toString();

    }

}

