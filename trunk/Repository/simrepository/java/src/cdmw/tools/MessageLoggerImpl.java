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


package cdmw.tools;

/**
 * This class provides a basic logger. 
 *
 * Features:
 * Log method is not thread-safe.
 *
 */
public class MessageLoggerImpl 
    extends com.thalesgroup.CdmwSimulatedRepository.MessageLoggerPOA {

    /**
     * The current POA
     */
    private org.omg.PortableServer.POA poa;
    
    /**
     *  PrintStream to log the data
     */
    private java.io.PrintStream os;

    /**
     * Constructor
     *
     * @param parent Parent POA to use
     * @param os PrintStream to log the data
     */ 
    public MessageLoggerImpl(org.omg.PortableServer.POA parent,
        String filename) {
        this.poa = parent;
        try {
            this.os = new java.io.PrintStream(
                new java.io.FileOutputStream(filename));
        } catch(java.io.FileNotFoundException fnfe) {}
    }    
    
    /**
     * Return the default POA
     *
     * @return The default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwSimulatedRepository/MessageLogger/log:1.0
    //
    /**
     * Log data with the specified header and body.
     *
     * @param header Message header of the logged data.
     * @param body Message body of the logged data.
     */
    public void log(String header,String body) {
        os.println("<SIM_Logger header=\"" + header + "\" >\n"
            + "\t<message>" + body + "</message>\n" + "</SIM_Logger>");
        os.flush();
    }

}

