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


package cdmw.common;

/**
 * Base class for CDMW exceptions
 *
 */
public class Exception extends java.lang.Exception {

    /**
     * List of severity types
     */
    public final static String[] SEVERITY = {
        "SAFE",
        "FATAL",
        "UNKNOWN"
    };

    /**
     * Safe severity type.
     */
    public final static int SAFE         = 0;
    
    /**
     * Fatal severity type
     */
    public final static int FATAL         = 1;
    
    /**
     * Unknown severity type
     */
    public final static int UNKNOWN     = 2;

    /**
     * Severity of the exception thrown.
     */
    private int severity;

    /**
     * File throwing the exception.
     */
    private String file;
    
    /**
     * Line throwing the exception.
     */
    private int line;

    /**
     * Exception constructor.
     *
     * @param severity Severity of the current exception
     * @param message Information message
     */
    public Exception(int severity, String message) {
        super(message);
        this.severity = severity;
    }

    /**
     * Returns the information message.
     *
     * @return String containing information about the exception.
     */
    public String what() {
        return getMessage();
    }

    /**
     * Set the file throwing the exception.
     *
     * @param file The file throwing the exception
     */
    public void setFile(String file) {
        this.file = file;
    }

    /**
     * Set the line throwing the exception.
     *
     * @param line The line throwing the exception
     */
    public void setLine(int line) {
        this.line = line;
    }

}

