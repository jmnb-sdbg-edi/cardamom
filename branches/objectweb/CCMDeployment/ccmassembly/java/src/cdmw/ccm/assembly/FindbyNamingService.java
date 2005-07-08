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

import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;


/**
 * Class representing a findby XML element with a 
 * namingservice child XML element.
 * For object retrieval, it resolve the specified name from the Naming Service.
 */
class FindbyNamingService
    implements Findby
{

    /**
     * Reference to the root context of the NamingService.
     */
    private NamingContext nameService;
    
    /**
     * Registration name of the object into the Naming Service.
     */
    private String name;


    /**
     * Constructor.
     *
     * @param nameService the root context of the Naming Service.
     * @param name the registration name of the object to be found in the Naming Service.
     */
    public FindbyNamingService(NamingContext nameService,
                               String name)
    {
        if (nameService == null) {
            throw new NullPointerException("nameService is null");
        }
        if (name == null) {
            throw new NullPointerException("name is null");
        }

        this.nameService = nameService;
        this.name = name;
    }

    /**
     * Returns the String representation of this FindbyNamingService object.
     *
     * @return the String representation of this FindbyNamingService object.
     */
    public String toString() {
        return "findbyName(" + name + ")";
    }

    /**
     * Returns the registration name of the object to be found.
     *
     * @return the registration name of the object to be found.
     */
    String getName() {
        return name;
    }
    
        

    /**
     * Resolves the registration name from the Naming Service
     * and returns the found CORBA object.
     *
     * @return the CORBA object registered with the name into the Naming Service.
     * 
     * @exception ObjectRef.RetrieveException if the CORBA object cannot
     *            be retrieved.
     */
    public org.omg.CORBA.Object retrieveObject()
        throws ObjectRef.RetrieveException
    {
        System.out.println("ASSEMBLY INFO: find object " + name);

        try {
            cdmw.namingandrepository.NamingInterface namingInterface =
                new cdmw.namingandrepository.NamingInterface(nameService);
            
            return namingInterface.resolve(name);

        } catch (NotFound e) {
            throw new ObjectRef.RetrieveException(
                "name " + name + " not found in NamingService");
        } catch (CannotProceed e) {
            throw new ObjectRef.RetrieveException(
                "cannot proceed to resolve name " + name);
        } catch (InvalidName e) {
            throw new ObjectRef.RetrieveException(
                "name " + name + " is not valid");
        }
    }


}

