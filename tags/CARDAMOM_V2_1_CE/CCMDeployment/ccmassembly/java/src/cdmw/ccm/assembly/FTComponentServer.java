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


/**
 * Class representing a fault tolerant ComponentServer.
 */
class FTComponentServer extends ComponentServer 
{
    
    /**
     * Boolean set to true if the components of this ComponentServer
     * have to be primaries at creation time.
     */
    private boolean isPrimary = false;
    
    
    /**
     * Constructor.
     *
     * @param id Identifier of the ComponentServer (empty string accepted)
     * @param parentHost parent HostCollocation of the ComponentServer.
     * @param processDestination process destination of ComponentServer.
     * @param primary true if ComponentServer should be primary at assembly creation
     */
    public FTComponentServer (String id,
                              HostCollocation parentHost,
                              String processDestination,
                              boolean primary)
        throws CreateFailure
    {
        super(id, parentHost, processDestination);
        this.isPrimary = primary;
        System.out.println("       (" + getId() + " is fault tolerant and " +
                           (isPrimary ? "primary)" : "backup)")
                           );
    }
    

    
    /**
     * Retuns true if this ComponentServer has previously been set
     * as primary.
     *
     * @return true if this FTComponentServer is primary.
     */
    public boolean isPrimary() {
        return isPrimary;
    }
    
}
