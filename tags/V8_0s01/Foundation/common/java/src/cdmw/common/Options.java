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


package cdmw.common;

/**
 * This class provides standard CDMW options for command lines.
 *
 */
public class Options {

    /**
     * Command Line option for CDMW Callback IOR.
     * Used by ApplicationAgent to acknowledge its startup.
     */
    public static final String CALLBACK_IOR_OPTION = "--CdmwCallbackIor";

    /**
     * Command Line option for CDMW Process Name
     */
    public static final String PROCESS_NAME_OPTION = "--CdmwProcessName";

    /**
     * Command Line option for CDMW Process Port
     */
    public static final String PROCESS_PORT_OPTION = "--CdmwProcessPort";

    /**
     * Command Line option for CDMW XML file for use
     * during Init of CDMW Processes
     */
    public static final String PROCESS_XML_FILE_OPTION = "--CdmwXMLFile";

    /**
     * Command Line option for validating XML files
     * (By default, XML files are not validated)
     */
    public static final String VALIDATE_XML_OPTION = "--validateXML";

    /**
     * Command Line option for Cdmw Localisation Service location
     * (Current option only holds the port number for use for binding
     * objects for corbaloc keys)
     */
    public static final String LOCALISATION_SERVICE_OPTION
        = "--CdmwLocalisationService";

    /**
     * Command Line option for specifying the full path to a log file
     */
    public static final String LOG_FILE_OPTION = "--CdmwLogFile";
    
    /**
     * Command Line option for specifying the use of DefaultHomeRegistration 
     * instead of the CDMW StandardHomeRegistration.
     */
    public static final String DEFAULT_HOME_REGISTRATION_OPTION = 
        "--DefaultHomeRegistration";

}

