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


package cdmw.lifecycle;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered;
import com.thalesgroup.CdmwNamingAndRepository.InvalidName;


/**
 * This class provides Guard objects to be used whenever a name
 * is registered within a NameDomain but should be 'unregistered'
 * in case an exception is raised.
 */
public class ObjectRegistrationGuard {

    /**
     * Reference to the NameDomain where name is registered.
     */
    private NameDomain domain;

    /**
     * Object Key of the object to unregister from the NameDomain
     */
    private String name;

    /**
     * Flag to indicate whether the desctructor
     * should unregister the name or not.
     */
    private boolean release;

    /**
     * Constructor
     *
     * @param domain Reference to the NameDomain where <I>name</I> is registered
     * @param name Object key registered in <I>domain</I>
     */
    public ObjectRegistrationGuard(NameDomain domain,String name) {

        this.domain = domain;
        this.name = name;
        this.release = true;

    }

    /**
     * Unregisters the name in the NameDomain if not canceled.
     */
    public void destroy() {
        if (release) {
            try {
                domain.release_name(name);
            } catch (NotRegistered nr) {
                // This should not happen (TODO)
            } catch (InvalidName in) {
                // This should not happen either (TODO)
            }
        }
    }

    /**
     * Protected Constructor. It initializes variables.
     */
    protected ObjectRegistrationGuard() {
        this.domain = null;
        this.name = "";
        this.release = false;
    }

    /**
     * This operation cancels unregistration of the name
     * at object destruction.
     */
    public void cancel() {
        this.release = false;
    }

}

