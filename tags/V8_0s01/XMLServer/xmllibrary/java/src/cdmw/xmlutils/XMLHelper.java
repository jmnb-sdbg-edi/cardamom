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


package cdmw.xmlutils;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

import cdmw.xmlutils.DocumentDescriptionFactoryImpl;
import cdmw.xmlutils.ElementDescriptionFactoryImpl;
import cdmw.xmlutils.AttrDescriptionFactoryImpl;
import cdmw.xmlutils.TextDescriptionFactoryImpl;
import cdmw.xmlutils.CDATASectionDescriptionFactoryImpl;

/**
 * This class provides helper functions for this CSC.
 *
 */
public class XMLHelper {

    /**
     * This function converts a String object to a short array.
     *
     * @param string The String object to be converted.
     * @return The short array corresponding to the string.
     */
    public static short[] toShortArray(String string) {
    
        byte[] bytes = string.getBytes();
        short[] result = new short[bytes.length];
        for(int i=0; i<result.length; i++) {
            result[i] = (new Byte(bytes[i])).shortValue();
        }
        return result;
    
    }
    
    /**
     * This function converts a short array to a String object.
     *
     * @param array The short array to be converted.
     * @return The string corresponding to the short array.
     */
    public static String toString(short[] array) {
        
        byte[] bytes = new byte[array.length];
        for(int i=0; i<bytes.length; i++) {
            bytes[i] = (new Short(array[i])).byteValue();
        }
        return new String(bytes);
        
    }
    

    /**
     * Creates and installs valuetypes factories.
     *
     * @param orb A valid reference to an ORB pseudo-object
     */
    public static void registerValueFactories(org.omg.CORBA.ORB orb) {
        
        //
        // Create & install value factory - if we forget to do this,
        // the client will receive a MARSHAL exception when attempting
        // to transmit a valuetype.
        //
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            com.thalesgroup.CdmwXML.DocumentDescriptionHelper.id(), 
                new DocumentDescriptionFactoryImpl());
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            com.thalesgroup.CdmwXML.ElementDescriptionHelper.id(), 
                new ElementDescriptionFactoryImpl());
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            com.thalesgroup.CdmwXML.AttrDescriptionHelper.id(), 
                new AttrDescriptionFactoryImpl());
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            com.thalesgroup.CdmwXML.TextDescriptionHelper.id(), 
                new TextDescriptionFactoryImpl());
        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            com.thalesgroup.CdmwXML.CDATASectionDescriptionHelper.id(), 
            new CDATASectionDescriptionFactoryImpl());
        
    }

}

