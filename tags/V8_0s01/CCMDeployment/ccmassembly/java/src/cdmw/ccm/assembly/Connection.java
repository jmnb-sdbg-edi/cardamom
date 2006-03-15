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


package cdmw.ccm.assembly;

import org.omg.Components.CreateFailure;
import org.omg.Components.RemoveFailure;


/**
 * Abstract class representing a connection between components.
 */
abstract class Connection {
    
    /**
     * Identifier of the connection, as specified into the connectinterface
     * or connectevent XML element of the .cad file.
     */
    private String id;
    
    /**
     * Boolean set to true if the connection has been established.
     */
    protected boolean isActivated;
    
    
    /**
     * Constructor.
     *
     * @param id the identitifer of the connection.
     */
    public Connection(String id) {
        if (id == null) {
            throw new NullPointerException("id is null");
        }
        this.id = id;
        isActivated = false;
    }
    
    /**
     * Returns the Connection's identifier.
     *
     * @return the Connection's identifier.
     */
    public String getId() {
        return id;
    }
    
    /**
     * Returns true is the connection has been established.
     *
     * @return true is the connection has been established.
     */
    public boolean isActivated() {
        return isActivated;
    }
    
    /**
     * Establishes the connection.
     *
     * @exception CreateFailure if the connection establishment fails.
     */
    public abstract void activate()
        throws CreateFailure;

    
    /**
     * Removes the connection.
     *
     * @exception RemoveFailure if the connection removal fails.
     */
    public abstract void deactivate()
        throws RemoveFailure;
    
    
}


