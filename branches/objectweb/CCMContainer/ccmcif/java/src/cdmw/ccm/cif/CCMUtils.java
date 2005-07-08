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


package cdmw.ccm.cif;

public class CCMUtils {
    
    /**
     * Creation of facet oid and read facet oid
     * 
     * @param componentOid The component object id
     * @param name facet_oid / local consumer oid
     * @param ckValue subscription cookie string for consumer oid
     * @return The newly created object id
     */
    public static byte[] createOid(
        String componentOid, 
        String name, 
        String ckValue) {
            
        if (ckValue == null) ckValue = "";
        String oidStr = componentOid + "/" + name;
    
        if (ckValue.length() > 0) {
            System.out.println("ck_value not empty");
            // creation of a local push consumer oid: add cookie value
            oidStr = oidStr + "/" + ckValue;
        }

        return oidStr.getBytes();
        
    }
    
    /**
     * Creation of facet oid and read facet oid
     * 
     * @param componentOid The component object id
     * @param name facet_oid / local consumer oid
     * @return The newly created object id
     */
    public static byte[] createOid(
        String componentOid, 
        String name) {
        return createOid(componentOid, name, null);
    }
    
    /**
     * Returns a list of string: 
     * [0] = component oid, 
     * [1] = facet_oid / local consumer oid,
     * [2] = subscription cookie string for consumer oid
     * 
     * @param oid Object Id to 
     * @return A vector containing strings as specified 
     */
    public static java.util.Vector readOid(String oid) {
        
        java.util.Vector nameVector = new java.util.Vector(3);
        String componentOid = oid;
        int endCompOidPos = componentOid.indexOf('/');

        while (endCompOidPos > -1) {
            // get name after '/'
            String tmpStr = componentOid.substring(0, endCompOidPos);
            nameVector.add(tmpStr);

            componentOid = componentOid.substring(
                endCompOidPos + 1, componentOid.length());
            endCompOidPos = componentOid.indexOf('/');
        }

        nameVector.add(componentOid);
        return nameVector;
        
    }

}

