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


package cdmw.ccm.assembly;

/**
 * Class representing a findby XML element with a 
 * stringifiedobjectref child XML element.
 * For object retrieval, the ORB and the object reference are used.
 */
class FindbyStringifiedObject
    implements Findby
{

    /**
     * Stringified reference to the object to be found.
     */
    private String stringifiedObject;
    
    /**
     * The ORB instance.
     */
    private org.omg.CORBA.ORB orb;


    /**
     * Constructor.
     *
     * @param stringifiedObject the Stringified reference 
     *        to the object to be found.
     * @param orb the ORB instance.
     */
    public FindbyStringifiedObject(String stringifiedObject,
                                   org.omg.CORBA.ORB orb)
    {
        if (stringifiedObject == null) {
            throw new NullPointerException("ior is null");
        }
        if (orb == null) {
            throw new NullPointerException("orb is null");
        }

        this.stringifiedObject = stringifiedObject;
        this.orb = orb;
    }

    /**
     * Returns the String representation of this FindbyStringifiedObject.
     *
     * @return the String representation of this FindbyStringifiedObject.
     */
    public String toString() {
        return "ObjectRef(" + stringifiedObject + ")";
    }

    /**
     * Returns the stringified object reference.
     *
     * @return the stringified object reference.
     */
    String getStringifiedObject() {
        return stringifiedObject;
    }
    

    /**
     * Uses the ORB to find the object with the specified reference.
     *
     * @return the CORBA object corresponding to the reference.
     * 
     * @exception ObjectRef.RetrieveException if the CORBA object cannot
     *            be retrieved.
     */
    public org.omg.CORBA.Object retrieveObject()
        throws ObjectRef.RetrieveException
    {
        System.out.println("ASSEMBLY INFO: find object " + stringifiedObject);

        try {
            org.omg.CORBA.Object obj = orb.string_to_object(stringifiedObject);

            if (obj == null) {
                throw new ObjectRef.RetrieveException(
                    "Cannot read " + stringifiedObject);
            }

            return obj;

        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new ObjectRef.RetrieveException(
                "Conversion of stringifiedObject " + stringifiedObject + 
                " failed: " + e.getMessage());
        }
    }


}



