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

import org.omg.Components.HomeFinder;
import org.omg.Components.HomeNotFound;


/**
 * Class representing a findby XML element with a 
 * homefinder child XML element.
 * For Home retrieval, a HomeFinder and a Home name are used.
 */
class FindbyHomeFinder
    implements Findby
{

    /**
     * Reference to the HomeFinder to be used.
     */
    private HomeFinder homeFinder;
    
    /**
     * Name of the home to be found.
     */
    private String homeName;

    
    /**
     * Constructor.
     *
     * @param homeFinder reference to the HomeFinder.
     * @param homeName the name of the home to be found.
     */
    public FindbyHomeFinder(HomeFinder homeFinder,
                            String homeName)
    {
        if (homeFinder == null) {
            throw new NullPointerException("homeFinder is null");
        }
        if (homeName == null) {
            throw new NullPointerException("homeName is null");
        }

        this.homeFinder = homeFinder;
        this.homeName = homeName;
    }

    /**
     * Returns the String representation of this FindbyHomeFinder object.
     *
     * @return the String representation of this FindbyHomeFinder object.
     */
    public String toString() {
        return "findHome(" + homeName + ")";
    }

    /**
     * Returns the Home's name.
     *
     * @return the Home's name.
     */
    String getHomeName() {
        return homeName;
    }


    /**
     * Uses the HomeFinder to find the home with the specified name.
     *
     * @return the CORBA object corresponding to the Home.
     * 
     * @exception ObjectRef.RetrieveException if the CORBA object cannot
     *            be retrieved.
     */
    public org.omg.CORBA.Object retrieveObject()
        throws ObjectRef.RetrieveException
    {
        System.out.println("ASSEMBLY INFO: find home " + homeName);

        try {
            return homeFinder.find_home_by_name(homeName);

        } catch (HomeNotFound e) {
            throw new ObjectRef.RetrieveException(
                "Home " + homeName + " not found");
        }
    }
}

