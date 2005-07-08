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


package cdmw.eventadmin;

import cdmw.common.Options;
import cdmw.ossupport.OS;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.orbsupport.CORBASystemExceptionWrapper;

/**
 * This class provides helper functions to initialize CDMW applications.
 *
 */
public class InitUtils {

    /**
     * Name of the Profile XML File option
     */
    public static String PROFILE_XML_FILE_OPTION = "--ProfileXMLFile";

    /**
     * Retrieves the CDMW Profile XML file name
     * from Program command line arguments.
     * 
     * @param args Arguments passed to the program
     * @return The name of current CDMW Profile XML file name
     * @exception CORBASystemExceptionWrapper Invalid parameter.
     * The following CDMW minor codes details tha reason: 
     * <B>BAD_PARAMProcessXMLFileNameMissing</B> when the CDMW
     * Profile XML file name is missing from the command line arguments
     */ 
    public static String getProfileXmlFile(String[] args)
        throws CORBASystemExceptionWrapper {

        // Get the XML file Name
        String filename = OS.getOptionValue(args, PROFILE_XML_FILE_OPTION);

        if ( filename.equals("no") || filename.equals("yes") ) {
            // Cannot Proceed with no file name
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMProcessXMLFileNameMissing,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO),
                "Cannot proceed with no file name");
        }

        return filename;
               
    }
    
    /**
     * Retrieves the CDMW Process XML initialisation file validation
     * option. It instructs the XML parser whether to validate
     * the XML document or not.
     * This will return <B>true</B> when XML document is validated whenever
     * VALIDATE_XML_OPTION appears in the argument list.
     *
     * @param args The arguments passed to the program
     * @return <B>true</B> if the XML document has to be validated
     * by the XML Parser, <B>false</B> otherwise.
     */
    public static boolean getXmlValidationOption(String[] args) {

        // XML document is validated whenever VALIDATE_XML_OPTION appears 
        // in the argument list.
        boolean validate = false;

        // Get the XML file Name
        String validateXmlOption =
            OS.getOptionValue( args, Options.VALIDATE_XML_OPTION);

        if ( validateXmlOption.equals("yes")
            || !validateXmlOption.equals("no") ) {
            validate = true;
        }

        return validate;

    }
    
}


